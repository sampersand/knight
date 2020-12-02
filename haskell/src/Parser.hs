{- The `Parser` type and its associated functions. -}
module Parser where

import Data.Char (isDigit, isSpace, isLower)
import Control.Applicative
import Data.Functor

-- | A Parser that's used within Knight.
-- 
-- There are standard library things that can probably do this better, but I
-- wrote it myself to learn how Haskell works.
newtype Parser a = Parser { parse :: String -> (String, Maybe a) }

{- Implementation for `Parser`. -}
instance Functor Parser where
  fmap f (Parser p) = Parser $ \s ->
    let (s', mf) = p s
    in (s', f <$> mf)

instance Applicative Parser where
  pure x = Parser $ \s -> (s, Just x)
  Parser pf <*> Parser px = Parser $ \s0 ->
    let (s1, mf) = pf s0
        (s2, mx) = px s1
        in (s2, mf <*> mx)

instance Alternative Parser where
  empty = Parser $ \s -> (s, Nothing)
  Parser p <|> Parser q = Parser $ \s -> 
    case (p s, q s) of
      (lhs@(_, Just _), _) -> lhs
      (_, rhs) -> rhs

instance Monad Parser where
  Parser p >>= f = Parser $ \s -> 
    let (s', thing) = p s
    in case thing of
      Just x -> let Parser pf = f x in pf s'
      Nothing -> (s', Nothing)

-- | The basic parsing function.
--
-- This parses a single character if the function returns True.
satisfy :: (Char -> Bool) -> Parser Char
satisfy f = Parser p
  where
    p (x:xs) | f x = (xs, Just x)
    p text         = (text, Nothing)

-- | Parses the given character.
char :: Char -> Parser Char
char c = satisfy (== c)

-- | Parses a single digit.
digit :: Parser Char
digit = satisfy isDigit

-- | Parses a single whitespace character.
space :: Parser Char
space = satisfy isSpace

-- | Parses a lower case letter.
lower :: Parser Char
lower = satisfy isLower

-- | Parses end of line (`\n`).
eol :: Parser ()
eol = void $ char '\n'

-- | Parses the given string.
string :: String -> Parser String 
string = traverse char
