module LearningMonads where

import Parser
import Knight.Parse
import Knight.Types

import Data.Char (isSpace)
import Control.Monad
import Control.Applicative

-- | Parses a single character---any will do!
anyChar :: Parser Char
anyChar = satisfy $ const True

-- | Parses a non-whitespace character
nonSpace :: Parser Char
nonSpace = satisfy $ not . isSpace

-- | Removes leading whitespace
lstrip :: Parser ()
lstrip = void $ many space

-- | Strips leading whitespace and parses nonwhitespace characters.
word :: Parser String
word = lstrip *> some nonSpace

-- | Strips leading whitespace and parses a number
amnt :: Parser Int
amnt = lstrip *> (read <$> some digit)

-- | The same as `replicateM`, except it can stop early if condition is met.
replicateMSatisfy :: (a -> Bool) -> Parser a -> Int -> Parser [a]
replicateMSatisfy cnd f cnt0 = 
    loop cnt0
  where
    loop cnt
        | cnt <= 0  = pure []
        | otherwise = do
          x <- f
          if cnd x
          then pure []
          else ((:) x) <$> loop (cnt - 1)

-- | Same as `numChars`, except it reads words (not chars) and will stop early if
-- "BREAK" is encountered.
numWords :: Parser [[String]]
numWords = many $ amnt >>= replicateMSatisfy (== "BREAK") word

-- | Parse a number and then read that many digits from the string. Rinse, repeat
-- until the end of the string is hit.
numChars :: Parser [String]
numChars = many $ amnt >>= (flip replicateM anyChar)

main :: IO ()
main = do
  print $ snd $ parse numChars "3hi42xs8fhdb35(94-$w:"
  print $ snd $ parse numWords
    "3 hello world !! 5 cut short BREAK 3 hello there bob"
