#include "weave/system/Process.hxx"
#include "weave/bugcheck/Assert.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <unistd.h>
#include <wordexp.h>
#include <spawn.h>
#include <sys/wait.h>
#include <poll.h>

WEAVE_EXTERNAL_HEADERS_END

namespace weave::system
{
    std::optional<int> Execute(
        const char* path,
        const char* args,
        const char* working_directory,
        std::string& output,
        std::string& error)
    {
        WEAVE_ASSERT(path != nullptr);

        std::string full_params = (args != nullptr)
            ? fmt::format("\"{}\" {}", path, args)
            : fmt::format("\"{}\"", path);

        wordexp_t exp_status{};

        int exp_result = wordexp(full_params.c_str(), &exp_status, 0);

        if (exp_result != 0)
        {
            return {};
        }

        int pipeOutput[2]{};
        int pipeError[2]{};

        if (pipe(pipeOutput) != 0)
        {
            return std::nullopt;
        }

        if (pipe(pipeError) != 0)
        {
            close(pipeOutput[0]);
            close(pipeOutput[1]);
            return std::nullopt;
        }

        pid_t process_id{};

        posix_spawn_file_actions_t files{};
        posix_spawn_file_actions_init(&files);

        posix_spawn_file_actions_addclose(&files, pipeOutput[0]);
        posix_spawn_file_actions_addclose(&files, pipeError[0]);
        posix_spawn_file_actions_adddup2(&files, pipeOutput[1], STDOUT_FILENO);
        posix_spawn_file_actions_adddup2(&files, pipeError[1], STDERR_FILENO);

#if defined(__USE_MISC)
        if (working_directory != nullptr)
        {
            posix_spawn_file_actions_addchdir_np(&files, working_directory);
        }
#endif

        int spawn_result = posix_spawnp(
            &process_id,
            path,
            &files,
            nullptr,
            exp_status.we_wordv,
            nullptr);

        int32_t exit_code{-1};
        if (spawn_result == 0)
        {
            close(pipeOutput[1]);
            close(pipeError[1]);

            pollfd pfd[2]{
                {pipeOutput[0], POLLIN},
                {pipeError[0], POLLIN},
            };

            size_t constexpr buffer_size = 4096;

            std::unique_ptr<char[]> buffer = std::make_unique<char[]>(buffer_size);

            while (poll(pfd, 2, -1) > 0)
            {
                if (pfd[0].revents & POLLIN)
                {
                    ssize_t processed = read(pfd[0].fd, buffer.get(), buffer_size);
                    if (processed != 0)
                    {
                        output.append(buffer.get(), processed);
                    }
                }
                else if (pfd[1].revents & POLLIN)
                {
                    ssize_t processed = read(pfd[1].fd, buffer.get(), buffer_size);
                    if (processed != 0)
                    {
                        error.append(buffer.get(), processed);
                    }
                }
                else
                {
                    break;
                }
            }

            waitpid(process_id, &exit_code, 0);
        }

        wordfree(&exp_status);
        posix_spawn_file_actions_destroy(&files);

        return exit_code;
    }
}
