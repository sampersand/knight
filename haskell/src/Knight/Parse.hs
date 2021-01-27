{- | Parsing Knight `Value`s. -}
module Knight.Parse where

import Knight.Types
import Parser

import Control.Applicative
import Data.Functor
import Data.Foldable

-- | Parses a Number.
number :: Parser Integer
number = read <$> some digit

-- | Parses a piece of Text, which should be surrounded by `q`.
text :: Char -> Parser String
text q = char q *> (many $ satisfy (/= q)) <* char q

-- | Parses a Variable, which should match `[a-z_0-9]+`.
variable :: Parser String
variable = do
  first <- lower <|> char '_'
  rest <- many $ asum [lower, digit, char '_']
  pure $ first : rest


-- | Parses a Literal value within Quest.
literal :: Parser Literal
literal = asum
  [ Null <$ string "null"
  , Bool True <$ string "true"
  , Bool False <$ string "false"
  , Num <$> number
  , Text <$> (text '\'' <|> text '"')
  ]

-- | A helper function that allows functions to be called via their full name,
-- or the first character.
--
-- If `a` is only a single character, that will be the only thing that's parsed.
named :: a -> String -> Parser a
named t (c:[]) = t <$ void (char c)
named t text@(c:_) = t <$ (void (string text) <|> void (char c)) -- TODO: shortened names

nullary :: Parser NullaryFn
nullary = asum 
  [ Variable <$> variable
  , Literal <$> literal
  , named Prompt "PROMPT"
  , named Random "RANDOM"
  ]

-- | Parse the unary functions.
unary :: Parser UnaryFn
unary = asum
  [ named Block "BLOCK"
  , named Call "CALL"
  , named Output "OUTPUT"
  , named Quit "QUIT"
  , named Eval "Eval"
  , named System "`"
  , named Length "LENGTH"
  , named Not "!"
  ]

-- | Parse the binary functions.
binary :: Parser BinaryFn
binary = asum
  [ named While "WHILE"
  , named Endl ";"
  , named Assign "="
  , named Add "+"
  , named Sub "-"
  , named Mul "*"
  , named Div "/"
  , named Pow "^"
  , named Eql "?"
  , named Lth "<"
  , named Gth ">"
  , named And "&"
  , named Or "|" ]

-- | Parse the ternary functions.
ternary :: Parser TernaryFn
ternary = asum [named If "IF", named Get "GET"]

-- | Parse the quaternary functions.
quaternary :: Parser QuaternaryFn
quaternary = named Set "SET"

-- | Removes comments, which start with `#` and go to the end of the line.
stripComment :: Parser ()
stripComment = char '#' *> many (satisfy (/= '\n')) *> eol

-- | Removes whitespace.
stripWhitespace :: Parser ()
stripWhitespace = void $ some space

-- | Removes parenthesis from Quest, as they're treated as whitespace.
stripParens :: Parser ()
stripParens = void $ asum $ char <$> "()[]{}"

-- | Removes all comments, whitespace, and parenthesis from the sourcecode.
strip :: Parser ()
strip = void $ many $ asum [stripComment, stripWhitespace, stripParens]

-- | Parses a single `Value`.
value :: Parser Value
value = strip *> asum
  [ Nullary <$> nullary
  , Unary   <$> unary   <*> value
  , Binary  <$> binary  <*> value <*> value
  , Ternary <$> ternary <*> value <*> value <*> value
  , Quaternary <$> quaternary <*> value <*> value <*> value <*> value
  ]
