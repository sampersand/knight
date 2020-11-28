import Control.Monad
import Data.List

sumOdd :: [Int] -> Int
sumOdd x = sum $ sum <$> filter (odd . length) (filter (flip isInfixOf x) (subsequences x))

main = print (sumOdd [1,4,2,5,3])
