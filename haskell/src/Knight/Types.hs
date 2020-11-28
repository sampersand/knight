{- | The types defined within Knight -}
module Knight.Types where

-- | A value in Knight.
--
-- Because Knight's entirely expression-based, everything is a Value.
data Value
  = Nullary NullaryFn
  | Unary UnaryFn Value
  | Binary BinaryFn Value Value
  | Ternary TernaryFn Value Value Value
  deriving (Show, Eq)

-- | A literal within Knight.
--
-- These can be thought of as functions that take no arguments.
data Literal
  = Null
  | Bool Bool
  | Num Integer
  | Text String
  deriving (Show, Eq)

data NullaryFn
  = Literal Literal
  | Variable String
  | Prompt
  deriving (Show, Eq)

-- | Functions that takes exactly one argument.
data UnaryFn
  = Not
  | FnDef
  | Call
  | Output
  | Quit
  | Eval
  | System
  deriving (Show, Eq)

-- | Functions that takes exactly two arguments.
data BinaryFn
  = Random
  | While
  | Endl | Assign
  | Add | Sub | Mul | Div | Mod | Pow
  | Lth | Gth | And | Or
  deriving (Show, Eq)

-- | Functions that takes exactly three arguments.
data TernaryFn
  = If
  deriving (Show, Eq)
