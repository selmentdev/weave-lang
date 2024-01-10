#ifndef WEAVE_SYNTAX_BEGIN_GROUP
#define WEAVE_SYNTAX_BEGIN_GROUP(name, value)
#endif

#ifndef WEAVE_SYNTAX_END_GROUP
#define WEAVE_SYNTAX_END_GROUP(name, value)
#endif

#ifndef WEAVE_SYNTAX
#define WEAVE_SYNTAX(name, value, spelling)
#endif

#ifndef WEAVE_SYNTAX_TRIVIA
#define WEAVE_SYNTAX_TRIVIA(name, value, spelling) WEAVE_SYNTAX(name, value, spelling)
#endif

#ifndef WEAVE_SYNTAX_TOKEN
#define WEAVE_SYNTAX_TOKEN(name, value, spelling) WEAVE_SYNTAX(name, value, spelling)
#endif

#ifndef WEAVE_SYNTAX_KEYWORD
#define WEAVE_SYNTAX_KEYWORD(name, value, spelling) WEAVE_SYNTAX(name, value, spelling)
#endif

#ifndef WEAVE_SYNTAX_TYPE_KEYWORD
#define WEAVE_SYNTAX_TYPE_KEYWORD(name, value, spelling) WEAVE_SYNTAX(name, value, spelling)
#endif

#ifndef WEAVE_SYNTAX_NODE
#define WEAVE_SYNTAX_NODE(name, value, spelling) WEAVE_SYNTAX(name, value, spelling)
#endif

// clang-format off

WEAVE_SYNTAX_BEGIN_GROUP(Token,                                     1000)
    WEAVE_SYNTAX_TOKEN(None,                                        1000, "[none]")
    WEAVE_SYNTAX_TOKEN(EndOfFile,                                   1001, "[end-of-file]")
    WEAVE_SYNTAX_TOKEN(IdentifierToken,                             1002, "[token:identifier]")
    WEAVE_SYNTAX_TOKEN(CharacterLiteralToken,                       1003, "[token:character-literal]")
    WEAVE_SYNTAX_TOKEN(StringLiteralToken,                          1004, "[token:string-literal]")
    WEAVE_SYNTAX_TOKEN(IntegerLiteralToken,                         1005, "[token:integer-literal]")
    WEAVE_SYNTAX_TOKEN(FloatLiteralToken,                           1006, "[token:float-literal]")
    WEAVE_SYNTAX_TOKEN(AmpersandAmpersandToken,                     1007, "&&")
    WEAVE_SYNTAX_TOKEN(AmpersandEqualsToken,                        1008, "&=")
    WEAVE_SYNTAX_TOKEN(AmpersandToken,                              1009, "&")
    WEAVE_SYNTAX_TOKEN(AsteriskEqualsToken,                         1010, "*=")
    WEAVE_SYNTAX_TOKEN(AsteriskToken,                               1011, "*")
    WEAVE_SYNTAX_TOKEN(AtToken,                                     1012, "@")
    WEAVE_SYNTAX_TOKEN(BackslashToken,                              1013, "\\")
    WEAVE_SYNTAX_TOKEN(BarBarToken,                                 1014, "||")
    WEAVE_SYNTAX_TOKEN(BarEqualsToken,                              1015, "|=")
    WEAVE_SYNTAX_TOKEN(BarToken,                                    1016, "|")
    WEAVE_SYNTAX_TOKEN(CaretEqualsToken,                            1017, "^=")
    WEAVE_SYNTAX_TOKEN(CaretToken,                                  1018, "^")
    WEAVE_SYNTAX_TOKEN(ColonColonToken,                             1019, "::")
    WEAVE_SYNTAX_TOKEN(ColonToken,                                  1020, ":")
    WEAVE_SYNTAX_TOKEN(CommaToken,                                  1021, ",")
    WEAVE_SYNTAX_TOKEN(DollarToken,                                 1022, "$")
    WEAVE_SYNTAX_TOKEN(DotDotDotToken,                              1023, "...")
    WEAVE_SYNTAX_TOKEN(DotDotToken,                                 1024, "..")
    WEAVE_SYNTAX_TOKEN(DotToken,                                    1025, ".")
    WEAVE_SYNTAX_TOKEN(EqualsEqualsToken,                           1026, "==")
    WEAVE_SYNTAX_TOKEN(EqualsGreaterThanToken,                      1027, "=>")
    WEAVE_SYNTAX_TOKEN(EqualsToken,                                 1028, "=")
    WEAVE_SYNTAX_TOKEN(ExclamationEqualsToken,                      1029, "!=")
    WEAVE_SYNTAX_TOKEN(ExclamationOpenParenToken,                   1030, "!(")
    WEAVE_SYNTAX_TOKEN(ExclamationToken,                            1031, "!")
    WEAVE_SYNTAX_TOKEN(GreaterGreaterGreaterToken,                  1032, ">>>")
    WEAVE_SYNTAX_TOKEN(GreaterThanEqualsToken,                      1033, ">=")
    WEAVE_SYNTAX_TOKEN(GreaterThanGreaterThanEqualsToken,           1034, ">>=")
    WEAVE_SYNTAX_TOKEN(GreaterThanGreaterThanToken,                 1035, ">>")
    WEAVE_SYNTAX_TOKEN(GreaterThanToken,                            1036, ">")
    WEAVE_SYNTAX_TOKEN(HashToken,                                   1037, "#")
    WEAVE_SYNTAX_TOKEN(LessLessLessToken,                           1038, "<<<")
    WEAVE_SYNTAX_TOKEN(LessThanEqualsToken,                         1039, "<=")
    WEAVE_SYNTAX_TOKEN(LessThanLessThanEqualsToken,                 1040, "<<=")
    WEAVE_SYNTAX_TOKEN(LessThanLessThanToken,                       1041, "<<")
    WEAVE_SYNTAX_TOKEN(LessThanToken,                               1042, "<")
    WEAVE_SYNTAX_TOKEN(MinusEqualsToken,                            1043, "-=")
    WEAVE_SYNTAX_TOKEN(MinusGreaterThanToken,                       1044, "->")
    WEAVE_SYNTAX_TOKEN(MinusMinusToken,                             1045, "--")
    WEAVE_SYNTAX_TOKEN(MinusToken,                                  1046, "-")
    WEAVE_SYNTAX_TOKEN(PercentEqualsToken,                          1047, "%=")
    WEAVE_SYNTAX_TOKEN(PercentToken,                                1048, "%")
    WEAVE_SYNTAX_TOKEN(PlusEqualsToken,                             1049, "+=")
    WEAVE_SYNTAX_TOKEN(PlusPlusToken,                               1050, "++")
    WEAVE_SYNTAX_TOKEN(PlusToken,                                   1051, "+")
    WEAVE_SYNTAX_TOKEN(QuestionQuestionToken,                       1052, "??")
    WEAVE_SYNTAX_TOKEN(QuestionToken,                               1053, "?")
    WEAVE_SYNTAX_TOKEN(SemicolonToken,                              1054, ";")
    WEAVE_SYNTAX_TOKEN(SlashEqualsToken,                            1055, "/=")
    WEAVE_SYNTAX_TOKEN(SlashToken,                                  1056, "/")
    WEAVE_SYNTAX_TOKEN(TildeToken,                                  1057, "~")
    WEAVE_SYNTAX_TOKEN(OpenBraceToken,                              1058, "{")
    WEAVE_SYNTAX_TOKEN(OpenBracketToken,                            1059, "[")
    WEAVE_SYNTAX_TOKEN(OpenParenToken,                              1060, "[")
    WEAVE_SYNTAX_TOKEN(CloseBraceToken,                             1061, "}")
    WEAVE_SYNTAX_TOKEN(CloseBracketToken,                           1062, "]")
    WEAVE_SYNTAX_TOKEN(CloseParenToken,                             1063, ")")
WEAVE_SYNTAX_END_GROUP(Token,                                       1063)

WEAVE_SYNTAX_BEGIN_GROUP(Trivia,                                    2000)
    WEAVE_SYNTAX_TRIVIA(WhitespaceTrivia,                           2000, "[trivia:whitespace]")
    WEAVE_SYNTAX_TRIVIA(EndOfLineTrivia,                            2001, "[trivia:end-of-line]")
    WEAVE_SYNTAX_TRIVIA(SingleLineCommentTrivia,                    2002, "[trivia:single-line-comment]")
    WEAVE_SYNTAX_TRIVIA(MultiLineCommentTrivia,                     2003, "[trivia:multi-line-comment]")
    WEAVE_SYNTAX_TRIVIA(SingleLineDocumentationTrivia,              2004, "[trivia:single-line-documentation]")
    WEAVE_SYNTAX_TRIVIA(MultiLineDocumentationTrivia,               2005, "[trivia:multi-line-documentation]")
WEAVE_SYNTAX_END_GROUP(Trivia,                                      2005)

WEAVE_SYNTAX_BEGIN_GROUP(TypeKeyword,                               3000)
    // Logic & untyped types
    WEAVE_SYNTAX_TYPE_KEYWORD(VoidTypeKeyword,                      3000, "void")
    WEAVE_SYNTAX_TYPE_KEYWORD(BoolTypeKeyword,                      3001, "bool")
    WEAVE_SYNTAX_TYPE_KEYWORD(ByteTypeKeyword,                      3002, "byte")

    // Character types
    WEAVE_SYNTAX_TYPE_KEYWORD(CharTypeKeyword,                      3003, "char")
    WEAVE_SYNTAX_TYPE_KEYWORD(Char8TypeKeyword,                     3004, "char8")
    WEAVE_SYNTAX_TYPE_KEYWORD(Char16TypeKeyword,                    3005, "char16")
    WEAVE_SYNTAX_TYPE_KEYWORD(Char32TypeKeyword,                    3006, "char32")

    // Floating point types
    WEAVE_SYNTAX_TYPE_KEYWORD(Float16TypeKeyword,                   3007, "float16")
    WEAVE_SYNTAX_TYPE_KEYWORD(Float32TypeKeyword,                   3008, "float32")
    WEAVE_SYNTAX_TYPE_KEYWORD(Float64TypeKeyword,                   3009, "float64")
    WEAVE_SYNTAX_TYPE_KEYWORD(Float128TypeKeyword,                  3010, "float128")

    // Decimal types
    WEAVE_SYNTAX_TYPE_KEYWORD(Decimal128TypeKeyword,                3011, "decimal128")
    WEAVE_SYNTAX_TYPE_KEYWORD(Decimal256TypeKeyword,                3012, "decimal256")

    // Signed integer types
    WEAVE_SYNTAX_TYPE_KEYWORD(Int8TypeKeyword,                      3013, "int8")
    WEAVE_SYNTAX_TYPE_KEYWORD(Int16TypeKeyword,                     3014, "int16") 
    WEAVE_SYNTAX_TYPE_KEYWORD(Int32TypeKeyword,                     3015, "int32")
    WEAVE_SYNTAX_TYPE_KEYWORD(Int64TypeKeyword,                     3016, "int64")
    WEAVE_SYNTAX_TYPE_KEYWORD(Int128TypeKeyword,                    3017, "int128")
    WEAVE_SYNTAX_TYPE_KEYWORD(IntSizeTypeKeyword,                   3018, "isize")
    WEAVE_SYNTAX_TYPE_KEYWORD(IntPtrTypeKeyword,                    3019, "intptr")
    WEAVE_SYNTAX_TYPE_KEYWORD(UInt8TypeKeyword,                     3020, "uint8")
    WEAVE_SYNTAX_TYPE_KEYWORD(UInt16TypeKeyword,                    3021, "uint16")
    WEAVE_SYNTAX_TYPE_KEYWORD(UInt32TypeKeyword,                    3022, "uint32")
    WEAVE_SYNTAX_TYPE_KEYWORD(UInt64TypeKeyword,                    3023, "uint64")
    WEAVE_SYNTAX_TYPE_KEYWORD(UInt128TypeKeyword,                   3024, "uint128")
    WEAVE_SYNTAX_TYPE_KEYWORD(UIntSizeTypeKeyword,                  3025, "usize")
    WEAVE_SYNTAX_TYPE_KEYWORD(UIntPtrTypeKeyword,                   3026, "uintptr")
WEAVE_SYNTAX_END_GROUP(TypeKeyword,                                 3026)

WEAVE_SYNTAX_BEGIN_GROUP(Keyword,                                   4000)
    WEAVE_SYNTAX_KEYWORD(AbstractKeyword,                           4000, "abstract")
    WEAVE_SYNTAX_KEYWORD(ActualKeyword,                             4001, "actual")
    WEAVE_SYNTAX_KEYWORD(AddKeyword,                                4002, "add")
    WEAVE_SYNTAX_KEYWORD(AddressOfKeyword,                          4003, "addressof")
    WEAVE_SYNTAX_KEYWORD(AddressSpaceKeyword,                       4004, "addrspace")
    WEAVE_SYNTAX_KEYWORD(AliasKeyword,                              4005, "alias")
    WEAVE_SYNTAX_KEYWORD(AlignasKeyword,                            4006, "alignas")
    WEAVE_SYNTAX_KEYWORD(AlignKeyword,                              4007, "align")
    WEAVE_SYNTAX_KEYWORD(AlignOfKeyword,                            4008, "alignof")
    WEAVE_SYNTAX_KEYWORD(AndEqualKeyword,                           4009, "and_eq")
    WEAVE_SYNTAX_KEYWORD(AndKeyword,                                4010, "and")
    WEAVE_SYNTAX_KEYWORD(ArrayKeyword,                              4011, "array")
    WEAVE_SYNTAX_KEYWORD(AsKeyword,                                 4012, "as")
    WEAVE_SYNTAX_KEYWORD(AssemblyKeyword,                           4013, "assembly")
    WEAVE_SYNTAX_KEYWORD(AssertKeyword,                             4014, "assert")
    WEAVE_SYNTAX_KEYWORD(AssumeKeyword,                             4015, "assume")
    WEAVE_SYNTAX_KEYWORD(AsyncKeyword,                              4016, "async")
    WEAVE_SYNTAX_KEYWORD(AtomicKeyword,                             4017, "atomic")
    WEAVE_SYNTAX_KEYWORD(AttributeKeyword,                          4018, "attribute")
    WEAVE_SYNTAX_KEYWORD(AutoKeyword,                               4019, "auto")
    WEAVE_SYNTAX_KEYWORD(AvailableKeyword,                          4020, "available")
    WEAVE_SYNTAX_KEYWORD(AwaitKeyword,                              4021, "await")
    WEAVE_SYNTAX_KEYWORD(BaseKeyword,                               4022, "base")
    WEAVE_SYNTAX_KEYWORD(BecomeKeyword,                             4023, "become")
    WEAVE_SYNTAX_KEYWORD(BitAndKeyword,                             4024, "bit_and")
    WEAVE_SYNTAX_KEYWORD(BitCastKeyword,                            4025, "bitcast")
    WEAVE_SYNTAX_KEYWORD(BitComplKeyword,                           4026, "bit_compl")
    WEAVE_SYNTAX_KEYWORD(BitFieldKeyword,                           4027, "bitfield")
    WEAVE_SYNTAX_KEYWORD(BitOrKeyword,                              4028, "bit_or")
    WEAVE_SYNTAX_KEYWORD(BitSetKeyword,                             4029, "bitset")
    WEAVE_SYNTAX_KEYWORD(BodyKeyword,                               4030, "body")
    WEAVE_SYNTAX_KEYWORD(BorrowKeyword,                             4031, "borrow")
    WEAVE_SYNTAX_KEYWORD(BoxKeyword,                                4032, "box")
    WEAVE_SYNTAX_KEYWORD(BreakKeyword,                              4033, "break")
    WEAVE_SYNTAX_KEYWORD(CallConvKeyword,                           4034, "callconv")
    WEAVE_SYNTAX_KEYWORD(CaseKeyword,                               4035, "case")
    WEAVE_SYNTAX_KEYWORD(CastKeyword,                               4036, "cast")
    WEAVE_SYNTAX_KEYWORD(CatchKeyword,                              4037, "catch")
    WEAVE_SYNTAX_KEYWORD(CheckedKeyword,                            4038, "checked")
    WEAVE_SYNTAX_KEYWORD(ChoiceKeyword,                             4039, "choice")
    WEAVE_SYNTAX_KEYWORD(ClassKeyword,                              4040, "class")
    WEAVE_SYNTAX_KEYWORD(ColumnMajorKeyword,                        4041, "columnmajor")
    WEAVE_SYNTAX_KEYWORD(CompileKeyword,                            4042, "compile")
    WEAVE_SYNTAX_KEYWORD(ConceptKeyword,                            4043, "concept")
    WEAVE_SYNTAX_KEYWORD(ConstKeyword,                              4044, "const")
    WEAVE_SYNTAX_KEYWORD(ConstraintKeyword,                         4045, "constraint")
    WEAVE_SYNTAX_KEYWORD(ConstructorKeyword,                        4046, "constructor")
    WEAVE_SYNTAX_KEYWORD(ContinueKeyword,                           4047, "continue")
    WEAVE_SYNTAX_KEYWORD(ContravariantKeyword,                      4048, "contravariant")
    WEAVE_SYNTAX_KEYWORD(CovariantKeyword,                          4049, "covariant")
    WEAVE_SYNTAX_KEYWORD(DataKeyword,                               4050, "data")
    WEAVE_SYNTAX_KEYWORD(DebuggerKeyword,                           4051, "debugger")
    WEAVE_SYNTAX_KEYWORD(DebugKeyword,                              4052, "debug")
    WEAVE_SYNTAX_KEYWORD(DefaultKeyword,                            4053, "default")
    WEAVE_SYNTAX_KEYWORD(DeferKeyword,                              4054, "defer")
    WEAVE_SYNTAX_KEYWORD(DelegateKeyword,                           4055, "delegate")
    WEAVE_SYNTAX_KEYWORD(DeleteKeyword,                             4056, "delete")
    WEAVE_SYNTAX_KEYWORD(DeprecatedKeyword,                         4057, "deprecated")
    WEAVE_SYNTAX_KEYWORD(DestructorKeyword,                         4058, "destructor")
    WEAVE_SYNTAX_KEYWORD(DisableKeyword,                            4059, "disable")
    WEAVE_SYNTAX_KEYWORD(DiscardableKeyword,                        4060, "discardable")
    WEAVE_SYNTAX_KEYWORD(DiscardKeyword,                            4061, "discard")
    WEAVE_SYNTAX_KEYWORD(DispatchKeyword,                           4062, "dispatch")
    WEAVE_SYNTAX_KEYWORD(DistinctKeyword,                           4063, "distinct")
    WEAVE_SYNTAX_KEYWORD(DoKeyword,                                 4064, "do")
    WEAVE_SYNTAX_KEYWORD(DowncastKeyword,                           4065, "downcast")
    WEAVE_SYNTAX_KEYWORD(DropKeyword,                               4066, "drop")
    WEAVE_SYNTAX_KEYWORD(DynamicKeyword,                            4067, "dynamic")
    WEAVE_SYNTAX_KEYWORD(EagerKeyword,                              4068, "eager")
    WEAVE_SYNTAX_KEYWORD(ElseKeyword,                               4069, "else")
    WEAVE_SYNTAX_KEYWORD(EnumKeyword,                               4070, "enum")
    WEAVE_SYNTAX_KEYWORD(EventKeyword,                              4071, "event")
    WEAVE_SYNTAX_KEYWORD(ExceptionKeyword,                          4072, "exception")
    WEAVE_SYNTAX_KEYWORD(ExceptKeyword,                             4073, "except")
    WEAVE_SYNTAX_KEYWORD(ExplicitKeyword,                           4074, "explicit")
    WEAVE_SYNTAX_KEYWORD(ExportKeyword,                             4075, "export")
    WEAVE_SYNTAX_KEYWORD(ExpressionKeyword,                         4076, "expression")
    WEAVE_SYNTAX_KEYWORD(ExtendKeyword,                             4077, "extend")
    WEAVE_SYNTAX_KEYWORD(ExtensionKeyword,                          4078, "extension")
    WEAVE_SYNTAX_KEYWORD(ExternKeyword,                             4079, "extern")
    WEAVE_SYNTAX_KEYWORD(FallthroughKeyword,                        4080, "fallthrough")
    WEAVE_SYNTAX_KEYWORD(FalseKeyword,                              4081, "false")
    WEAVE_SYNTAX_KEYWORD(FieldKeyword,                              4082, "field")
    WEAVE_SYNTAX_KEYWORD(FileKeyword,                               4083, "file")
    WEAVE_SYNTAX_KEYWORD(FinalKeyword,                              4084, "final")
    WEAVE_SYNTAX_KEYWORD(FinallyKeyword,                            4085, "finally")
    WEAVE_SYNTAX_KEYWORD(FixedKeyword,                              4086, "fixed")
    WEAVE_SYNTAX_KEYWORD(ForceKeyword,                              4087, "force")
    WEAVE_SYNTAX_KEYWORD(ForeachKeyword,                            4088, "foreach")
    WEAVE_SYNTAX_KEYWORD(ForeignKeyword,                            4089, "foreign")
    WEAVE_SYNTAX_KEYWORD(ForKeyword,                                4090, "for")
    WEAVE_SYNTAX_KEYWORD(FractionKeyword,                           4091, "fraction")
    WEAVE_SYNTAX_KEYWORD(FragmentKeyword,                           4092, "fragment")
    WEAVE_SYNTAX_KEYWORD(FriendKeyword,                             4093, "friend")
    WEAVE_SYNTAX_KEYWORD(FromKeyword,                               4094, "from")
    WEAVE_SYNTAX_KEYWORD(FunctionKeyword,                           4095, "function")
    WEAVE_SYNTAX_KEYWORD(GenericKeyword,                            4096, "generic")
    WEAVE_SYNTAX_KEYWORD(GetKeyword,                                4097, "get")
    WEAVE_SYNTAX_KEYWORD(GlobalKeyword,                             4098, "global")
    WEAVE_SYNTAX_KEYWORD(GotoKeyword,                               4099, "goto")
    WEAVE_SYNTAX_KEYWORD(GroupSharedKeyword,                        4100, "groupshared")
    WEAVE_SYNTAX_KEYWORD(GuardKeyword,                              4101, "guard")
    WEAVE_SYNTAX_KEYWORD(IfKeyword,                                 4102, "if")
    WEAVE_SYNTAX_KEYWORD(ImmutableKeyword,                          4103, "immutable")
    WEAVE_SYNTAX_KEYWORD(ImplementationKeyword,                     4104, "implementation")
    WEAVE_SYNTAX_KEYWORD(ImplicitKeyword,                           4105, "implicit")
    WEAVE_SYNTAX_KEYWORD(ImportKeyword,                             4106, "import")
    WEAVE_SYNTAX_KEYWORD(InfixKeyword,                              4107, "infix")
    WEAVE_SYNTAX_KEYWORD(InfixlKeyword,                             4108, "infixl")
    WEAVE_SYNTAX_KEYWORD(InfixrKeyword,                             4109, "infixr")
    WEAVE_SYNTAX_KEYWORD(InheritKeyword,                            4110, "inherit")
    WEAVE_SYNTAX_KEYWORD(InitKeyword,                               4111, "init")
    WEAVE_SYNTAX_KEYWORD(InjectKeyword,                             4112, "inject")
    WEAVE_SYNTAX_KEYWORD(InKeyword,                                 4113, "in")
    WEAVE_SYNTAX_KEYWORD(InlineKeyword,                             4114, "inline")
    WEAVE_SYNTAX_KEYWORD(InnerKeyword,                              4115, "inner")
    WEAVE_SYNTAX_KEYWORD(InOutKeyword,                              4116, "inout")
    WEAVE_SYNTAX_KEYWORD(InstanceKeyword,                           4117, "instance")
    WEAVE_SYNTAX_KEYWORD(InterfaceKeyword,                          4118, "interface")
    WEAVE_SYNTAX_KEYWORD(InternalKeyword,                           4119, "internal")
    WEAVE_SYNTAX_KEYWORD(InvariantKeyword,                          4120, "invariant")
    WEAVE_SYNTAX_KEYWORD(IsKeyword,                                 4121, "is")
    WEAVE_SYNTAX_KEYWORD(LabelKeyword,                              4122, "label")
    WEAVE_SYNTAX_KEYWORD(LazyKeyword,                               4123, "lazy")
    WEAVE_SYNTAX_KEYWORD(LetKeyword,                                4124, "let")
    WEAVE_SYNTAX_KEYWORD(LibraryKeyword,                            4125, "library")
    WEAVE_SYNTAX_KEYWORD(LikelyKeyword,                             4126, "likely")
    WEAVE_SYNTAX_KEYWORD(LinkSectionKeyword,                        4127, "linksection")
    WEAVE_SYNTAX_KEYWORD(LiteralKeyword,                            4128, "literal")
    WEAVE_SYNTAX_KEYWORD(LockKeyword,                               4129, "lock")
    WEAVE_SYNTAX_KEYWORD(LoopKeyword,                               4130, "loop")
    WEAVE_SYNTAX_KEYWORD(MacroKeyword,                              4131, "macro")
    WEAVE_SYNTAX_KEYWORD(ManagerKeyword,                            4132, "managed")
    WEAVE_SYNTAX_KEYWORD(MatchKeyword,                              4133, "match")
    WEAVE_SYNTAX_KEYWORD(MatrixKeyword,                             4134, "matrix")
    WEAVE_SYNTAX_KEYWORD(MemberKeyword,                             4135, "member")
    WEAVE_SYNTAX_KEYWORD(MetaKeyword,                               4136, "meta")
    WEAVE_SYNTAX_KEYWORD(MixinKeyword,                              4137, "mixin")
    WEAVE_SYNTAX_KEYWORD(ModuleKeyword,                             4138, "module")
    WEAVE_SYNTAX_KEYWORD(MoveKeyword,                               4139, "move")
    WEAVE_SYNTAX_KEYWORD(MutableKeyword,                            4140, "mutable")
    WEAVE_SYNTAX_KEYWORD(NameOfKeyword,                             4141, "nameof")
    WEAVE_SYNTAX_KEYWORD(NamespaceKeyword,                          4142, "namespace")
    WEAVE_SYNTAX_KEYWORD(NativeKeyword,                             4143, "native")
    WEAVE_SYNTAX_KEYWORD(NewKeyword,                                4144, "new")
    WEAVE_SYNTAX_KEYWORD(NewtypeKeyword,                            4145, "newtype")
    WEAVE_SYNTAX_KEYWORD(NoAliasKeyword,                            4146, "noalias")
    WEAVE_SYNTAX_KEYWORD(NodiscardKeyword,                          4147, "nodiscard")
    WEAVE_SYNTAX_KEYWORD(NoInlineKeyword,                           4148, "noinline")
    WEAVE_SYNTAX_KEYWORD(NoreturnKeyword,                           4149, "noreturn")
    WEAVE_SYNTAX_KEYWORD(NoSuspendKeyword,                          4150, "nosuspend")
    WEAVE_SYNTAX_KEYWORD(NotEqKeyword,                              4151, "not_eq")
    WEAVE_SYNTAX_KEYWORD(NotKeyword,                                4152, "not")
    WEAVE_SYNTAX_KEYWORD(NotNullKeyword,                            4153, "notnull")
    WEAVE_SYNTAX_KEYWORD(NullableKeyword,                           4154, "nullable")
    WEAVE_SYNTAX_KEYWORD(NullKeyword,                               4155, "null")
    WEAVE_SYNTAX_KEYWORD(OffsetOfKeyword,                           4156, "offsetof")
    WEAVE_SYNTAX_KEYWORD(OfKeyword,                                 4157, "of")
    WEAVE_SYNTAX_KEYWORD(OperatorKeyword,                           4158, "operator")
    WEAVE_SYNTAX_KEYWORD(OpqaueKeyword,                             4159, "opaque")
    WEAVE_SYNTAX_KEYWORD(OptionalKeyword,                           4160, "optional")
    WEAVE_SYNTAX_KEYWORD(OrEqualKeyword,                            4161, "or_eq")
    WEAVE_SYNTAX_KEYWORD(OrKeyword,                                 4162, "or")
    WEAVE_SYNTAX_KEYWORD(OuterKeyword,                              4163, "outer")
    WEAVE_SYNTAX_KEYWORD(OutKeyword,                                4164, "out")
    WEAVE_SYNTAX_KEYWORD(OverloadKeyword,                           4165, "overload")
    WEAVE_SYNTAX_KEYWORD(OverrideKeyword,                           4166, "override")
    WEAVE_SYNTAX_KEYWORD(PackageKeyword,                            4167, "package")
    WEAVE_SYNTAX_KEYWORD(PackedKeyword,                             4168, "packed")
    WEAVE_SYNTAX_KEYWORD(ParallelKeyword,                           4169, "parallel")
    WEAVE_SYNTAX_KEYWORD(ParamsKeyword,                             4170, "params")
    WEAVE_SYNTAX_KEYWORD(PartialKeyword,                            4171, "partial")
    WEAVE_SYNTAX_KEYWORD(PragmaKeyword,                             4172, "pragma")
    WEAVE_SYNTAX_KEYWORD(PreciseKeyword,                            4173, "precise")
    WEAVE_SYNTAX_KEYWORD(PrivateKeyword,                            4174, "private")
    WEAVE_SYNTAX_KEYWORD(PropertyKeyword,                           4175, "property")
    WEAVE_SYNTAX_KEYWORD(ProtectedKeyword,                          4176, "protected")
    WEAVE_SYNTAX_KEYWORD(PublicKeyword,                             4177, "public")
    WEAVE_SYNTAX_KEYWORD(PureKeyword,                               4178, "pure")
    WEAVE_SYNTAX_KEYWORD(RaiseKeyword,                              4179, "raise")
    WEAVE_SYNTAX_KEYWORD(RawKeyword,                                4180, "raw")
    WEAVE_SYNTAX_KEYWORD(ReadonlyKeyword,                           4181, "readonly")
    WEAVE_SYNTAX_KEYWORD(RecordKeyword,                             4182, "record")
    WEAVE_SYNTAX_KEYWORD(RecursiveKeyword,                          4183, "recursive")
    WEAVE_SYNTAX_KEYWORD(RefKeyword,                                4184, "ref")
    WEAVE_SYNTAX_KEYWORD(ReflectKeyword,                            4185, "reflect")
    WEAVE_SYNTAX_KEYWORD(RegisterKeyword,                           4186, "register")
    WEAVE_SYNTAX_KEYWORD(ReinterpretKeyword,                        4187, "reinterpret")
    WEAVE_SYNTAX_KEYWORD(RemoveKeyword,                             4188, "remove")
    WEAVE_SYNTAX_KEYWORD(RequiredKeyword,                           4189, "required")
    WEAVE_SYNTAX_KEYWORD(RequireKeyword,                            4190, "require")
    WEAVE_SYNTAX_KEYWORD(RequiresKeyword,                           4191, "requires")
    WEAVE_SYNTAX_KEYWORD(RestrictedKeyword,                         4192, "restricted")
    WEAVE_SYNTAX_KEYWORD(ResumeKeyword,                             4193, "resume")
    WEAVE_SYNTAX_KEYWORD(RetainKeyword,                             4194, "retain")
    WEAVE_SYNTAX_KEYWORD(ReturnKeyword,                             4195, "return")
    WEAVE_SYNTAX_KEYWORD(RowMajorKeyword,                           4196, "rowmajor")
    WEAVE_SYNTAX_KEYWORD(SafeKeyword,                               4197, "safe")
    WEAVE_SYNTAX_KEYWORD(SaturateKeyword,                           4198, "saturate")
    WEAVE_SYNTAX_KEYWORD(ScalarKeyword,                             4199, "scalar")
    WEAVE_SYNTAX_KEYWORD(ScopeKeyword,                              4200, "scope")
    WEAVE_SYNTAX_KEYWORD(SectionKeyword,                            4201, "section")
    WEAVE_SYNTAX_KEYWORD(SelectKeyword,                             4202, "select")
    WEAVE_SYNTAX_KEYWORD(SelfKeyword,                               4203, "self")
    WEAVE_SYNTAX_KEYWORD(SetKeyword,                                4204, "set")
    WEAVE_SYNTAX_KEYWORD(SharedKeyword,                             4205, "shared")
    WEAVE_SYNTAX_KEYWORD(SizeOfKeyword,                             4206, "sizeof")
    WEAVE_SYNTAX_KEYWORD(SliceKeyword,                              4207, "slice")
    WEAVE_SYNTAX_KEYWORD(StackallocKeyword,                         4208, "stackalloc")
    WEAVE_SYNTAX_KEYWORD(StaticKeyword,                             4209, "static")
    WEAVE_SYNTAX_KEYWORD(StructKeyword,                             4210, "struct")
    WEAVE_SYNTAX_KEYWORD(SuperKeyword,                              4211, "super")
    WEAVE_SYNTAX_KEYWORD(SuspendKeyword,                            4212, "suspend")
    WEAVE_SYNTAX_KEYWORD(SwitchKeyword,                             4213, "switch")
    WEAVE_SYNTAX_KEYWORD(SynchronizedKeyword,                       4214, "synchronized")
    WEAVE_SYNTAX_KEYWORD(TailCallKeyword,                           4215, "tailcall")
    WEAVE_SYNTAX_KEYWORD(TemplateKeyword,                           4216, "template")
    WEAVE_SYNTAX_KEYWORD(TestKeyword,                               4217, "test")
    WEAVE_SYNTAX_KEYWORD(ThenKeyword,                               4218, "then")
    WEAVE_SYNTAX_KEYWORD(ThisKeyword,                               4219, "this")
    WEAVE_SYNTAX_KEYWORD(ThreadLocalKeyword,                        4220, "threadlocal")
    WEAVE_SYNTAX_KEYWORD(ThrowKeyword,                              4221, "throw")
    WEAVE_SYNTAX_KEYWORD(TraitKeyword,                              4222, "trait")
    WEAVE_SYNTAX_KEYWORD(TraitsKeyword,                             4223, "traits")
    WEAVE_SYNTAX_KEYWORD(TransferKeyword,                           4224, "transfer")
    WEAVE_SYNTAX_KEYWORD(TransientKeyword,                          4225, "transient")
    WEAVE_SYNTAX_KEYWORD(TransmuteKeyword,                          4226, "transmute")
    WEAVE_SYNTAX_KEYWORD(TrueKeyword,                               4227, "true")
    WEAVE_SYNTAX_KEYWORD(TrustedKeyword,                            4228, "trusted")
    WEAVE_SYNTAX_KEYWORD(TryKeyword,                                4229, "try")
    WEAVE_SYNTAX_KEYWORD(TupleKeyword,                              4230, "tuple")
    WEAVE_SYNTAX_KEYWORD(TypeKeyword,                               4231, "type")
    WEAVE_SYNTAX_KEYWORD(TypeOfKeyword,                             4232, "typeof")
    WEAVE_SYNTAX_KEYWORD(UnalignedKeyword,                          4233, "unaligned")
    WEAVE_SYNTAX_KEYWORD(UniversalKeyword,                          4234, "universal")
    WEAVE_SYNTAX_KEYWORD(UnboxKeyword,                              4235, "unbox")
    WEAVE_SYNTAX_KEYWORD(UncheckedKeyword,                          4236, "unchecked")
    WEAVE_SYNTAX_KEYWORD(UndefinedKeyword,                          4237, "undefined")
    WEAVE_SYNTAX_KEYWORD(UniformKeyword,                            4238, "uniform")
    WEAVE_SYNTAX_KEYWORD(UnionKeyword,                              4239, "union")
    WEAVE_SYNTAX_KEYWORD(UnitKeyword,                               4240, "unit")
    WEAVE_SYNTAX_KEYWORD(UnlikelyKeyword,                           4241, "unlikely")
    WEAVE_SYNTAX_KEYWORD(UnmanagedKeyword,                          4242, "unmanaged")
    WEAVE_SYNTAX_KEYWORD(UnreachableKeyword,                        4243, "unreachable")
    WEAVE_SYNTAX_KEYWORD(UnsafeKeyword,                             4244, "unsafe")
    WEAVE_SYNTAX_KEYWORD(UnsizedKeyword,                            4245, "unsized")
    WEAVE_SYNTAX_KEYWORD(UpcastKeyword,                             4246, "upcast")
    WEAVE_SYNTAX_KEYWORD(UsingKeyword,                              4247, "using")
    WEAVE_SYNTAX_KEYWORD(UuidOfKeyword,                             4248, "uuidof")
    WEAVE_SYNTAX_KEYWORD(ValueKeyword,                              4249, "value")
    WEAVE_SYNTAX_KEYWORD(VarKeyword,                                4250, "var")
    WEAVE_SYNTAX_KEYWORD(VectorKeyword,                             4251, "vector")
    WEAVE_SYNTAX_KEYWORD(VendorKeyword,                             4252, "vendor")
    WEAVE_SYNTAX_KEYWORD(VersionKeyword,                            4253, "version")
    WEAVE_SYNTAX_KEYWORD(VirtualKeyword,                            4254, "virtual")
    WEAVE_SYNTAX_KEYWORD(VolatileKeyword,                           4255, "volatile")
    WEAVE_SYNTAX_KEYWORD(VtableKeyword,                             4256, "vtable")
    WEAVE_SYNTAX_KEYWORD(WeakKeyword,                               4257, "weak")
    WEAVE_SYNTAX_KEYWORD(WhenKeyword,                               4258, "when")
    WEAVE_SYNTAX_KEYWORD(WhereKeyword,                              4259, "where")
    WEAVE_SYNTAX_KEYWORD(WhileKeyword,                              4260, "while")
    WEAVE_SYNTAX_KEYWORD(WithKeyword,                               4261, "with")
    WEAVE_SYNTAX_KEYWORD(XorEqualKeyword,                           4262, "xor_eq")
    WEAVE_SYNTAX_KEYWORD(XorKeyword,                                4263, "xor")
    WEAVE_SYNTAX_KEYWORD(YieldKeyword,                              4264, "yield")
    WEAVE_SYNTAX_KEYWORD(BuiltinKeyword,                            4265, "builtin")
WEAVE_SYNTAX_END_GROUP(Keyword,                                     4265)

WEAVE_SYNTAX_BEGIN_GROUP(Node,                                      10000)
    WEAVE_SYNTAX_NODE(CompilationUnit,                              10000, "[node:compilation-unit]")
    WEAVE_SYNTAX_NODE(UsingDirective,                               10001, "[node:using-directive]")
    WEAVE_SYNTAX_NODE(MemberDeclaration,                            10002, "[node:member-declaration]")
    WEAVE_SYNTAX_NODE(NamespaceDeclaration,                         10003, "[node:namespace-declaration]")
WEAVE_SYNTAX_END_GROUP(Node,                                        10003)


// clang-format on

#ifdef WEAVE_SYNTAX_BEGIN_GROUP
#undef WEAVE_SYNTAX_BEGIN_GROUP
#endif

#ifdef WEAVE_SYNTAX_END_GROUP
#undef WEAVE_SYNTAX_END_GROUP
#endif

#ifdef WEAVE_SYNTAX
#undef WEAVE_SYNTAX
#endif

#ifdef WEAVE_SYNTAX_TRIVIA
#undef WEAVE_SYNTAX_TRIVIA
#endif

#ifdef WEAVE_SYNTAX_TOKEN
#undef WEAVE_SYNTAX_TOKEN
#endif

#ifdef WEAVE_SYNTAX_KEYWORD
#undef WEAVE_SYNTAX_KEYWORD
#endif

#ifdef WEAVE_SYNTAX_TYPE_KEYWORD
#undef WEAVE_SYNTAX_TYPE_KEYWORD
#endif

#ifdef WEAVE_SYNTAX_NODE
#undef WEAVE_SYNTAX_NODE
#endif
