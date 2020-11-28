{-# LANGUAGE FlexibleInstances, TypeApplications #-}

module Knight.Evaluate where

import Knight.Types
import qualified Knight.Parse
import Parser
import Evaluator

import System.Process
-- import System.Random
import GHC.IO.Handle
import Control.Monad.IO.Class
import Control.Applicative
import Data.Functor
import Data.Map
import Data.Maybe
import System.Exit

class ValueConvertible a where
  into :: a -> Value
  from :: Value -> EvaluatorM a

-- = <-- fix dumb sublime text formatting

instance ValueConvertible Bool where
  into = Nullary . Literal . Bool

  from (Nullary (Literal (Null))) = pure False
  from (Nullary (Literal (Bool b))) = pure b
  from (Nullary (Literal (Num n))) = pure (n /= 0)
  from (Nullary (Literal (Text t))) = pure (not $ Prelude.null t)
  from other = value other >>= from

instance ValueConvertible NullaryFn where
  into = Nullary
  from (Nullary n) = pure $ n
  from other = value other >>= from

instance ValueConvertible String where
  into = Nullary . Literal . Text

  from (Nullary (Literal (Null))) = pure "null"
  from (Nullary (Literal (Bool b))) = pure $ show b
  from (Nullary (Literal (Num n))) = pure $ show n
  from (Nullary (Literal (Text t))) = pure t
  from other = value other >>= from

instance ValueConvertible Integer where
  into = Nullary . Literal . Num

  from (Nullary (Literal (Null))) = pure 0
  from (Nullary (Literal (Bool b))) = pure $ if b then 1 else 0
  from (Nullary (Literal (Num n))) = pure n
  from (Nullary (Literal (Text t))) = pure $ read t -- will crash if `t` is invalid.
  from other = value other >>= from

nullValue :: Value
nullValue = Nullary $ Literal Null

nullary :: NullaryFn -> EvaluatorM Value
nullary (Variable var) = EvaluatorM $ \env ->
  pure (env, Right $ fromMaybe nullValue (env !? var))
nullary (Literal lit) = pure $ Nullary $ Literal lit
nullary (Prompt) = liftIO $ into <$> getLine

unary :: UnaryFn -> Value -> Evaluator
unary Not x = into . not <$> from x
unary FnDef x = pure x
unary Call x = value x >>= value
unary Output x = do
  text <- from @String x
  liftIO $ nullValue <$
    if not (Prelude.null text) && (last text) == '\\'
      then putStr $ init text 
      else putStrLn text
unary Quit x = do
  exitCode <- from x
  liftIO $ exitWith $
    if exitCode == 0
      then ExitSuccess
      else ExitFailure $ fromInteger exitCode
unary Eval x = do
  code <- from x
  let result = snd $ parse Knight.Parse.value code
  pure $ fromMaybe nullValue result
unary System x = do
  cmd <- from x
  let proc = (shell cmd){std_out=CreatePipe}
  (_, stdout, _, _) <- liftIO $ createProcess proc
  case stdout of
    Just s -> liftIO $ into <$> hGetContents s
    Nothing -> pure nullValue

binary :: BinaryFn -> Value -> Value -> Evaluator
-- binary Random start stop = do
--   start' <- from start
--   stop' <- from stop
--   error "todo: random" 
--   -- pure $ Nullary $ Num $ randomR (start' stop') 
binary While cond body = 
    loop
  where
    loop = do
      cond' <- from cond
      if cond'
        then value body *> loop
        else pure $ nullValue
binary Endl lhs rhs = value lhs *> value rhs
binary Assign (Nullary (Variable var)) contents = EvaluatorM $ \env -> do
  (env', contents') <- liftIO $ eval (value contents) env
  case contents' of
    Left l -> pure (env', Left l)
    Right r -> pure $ (insert var r env', Right r)
binary op lv rv = value lv >>= getLiteral >>= (go op)
  where
    go Add (Num l) = into . (l +) <$> from rv
    go Add (Text l) = into . (l ++) <$> from rv
    go Sub (Num l) = into . (l -) <$> from rv
    go Mul (Num l) = into . (l *) <$> from rv
    go Div (Num l) = do
      r <- from rv
      if r == 0
        then runtimeError "divide by zero"
        else pure $ into $ l `div` r
    go Mod (Num l) = into . (l `mod`) <$> from rv
    go Pow (Num l) = into . (l ^) <$> from @Integer rv
    go Lth (Num l) = into . (l <) <$> from rv
    go Lth (Text l) = into . (l <) <$> from rv
    go Gth (Num l) = into . (l >) <$> from rv
    go Gth (Text l) = into . (l >) <$> from rv
    go And _ = do
      l <- from lv
      r <- from rv
      pure $ into $ l && r
    go Or _ = do
      l <- from lv
      r <- from rv
      pure $ into $ l || r
    go op l = runtimeError $ "invalid " ++ show op ++ " operand: " ++ show l

runtimeError :: String -> EvaluatorM a
runtimeError err = EvaluatorM $ \env -> pure $ (env, Left err)

getLiteral :: Value -> EvaluatorM Literal
getLiteral (Nullary (Literal lit )) = pure lit
getLiteral other = value other >>= getLiteral

ternary :: TernaryFn -> Value -> Value -> Value -> Evaluator
ternary If cond true false = do
  cond' <- from cond
  value $ if cond' then true else false

value :: Value -> Evaluator
value (Nullary x) = nullary x
value (Unary f x) = unary f x
value (Binary f x y) = binary f x y
value (Ternary f x y z) = ternary f x y z
