{-# LANGUAGE FlexibleInstances #-}
module Main where

import Parser
import Evaluator
import qualified Knight.Parse
import qualified Knight.Evaluate

import Data.Maybe
import qualified Data.Map

main = do
  let
    Just code = snd $ parse Knight.Parse.value ("\
\ ; = fact F \
  \ I (< x 1) \
    \ 1 \
    \ ; = x (- x 1) \
      \ * (+ 1 x) (C fact) \
\ ; = x 10 \
\ O C fact \
      \" )
    env = Data.Map.empty :: Env

  eval (Knight.Evaluate.value code) env
  print ()
  -- let (_, value) = print $ eval $ fromMaybe $ snd $ parse value "* 3 + 4 2"
-- main = do
--   print $ parse random "3hi42xs8fhdb35(94-$w:"
--   print $ parse randomBreak "3 hello world !! 5 cut short BREAK 3 hello there kenobi"
