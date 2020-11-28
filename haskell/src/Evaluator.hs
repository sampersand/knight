module Evaluator where

import Knight.Types
import Data.Map

import Control.Applicative
import Control.Monad.IO.Class

type Env = Map String Value
newtype EvaluatorM a = EvaluatorM { eval :: Env -> IO (Env, Either String a) }
type Evaluator = EvaluatorM Value

-- {- Implementation for `EvaluatorM`. -}
instance Functor EvaluatorM where
  fmap f (EvaluatorM m) = EvaluatorM $ \env -> do
    (env', res) <- m env
    pure (env', f <$> res)

instance Applicative EvaluatorM where
  pure val = EvaluatorM $ \env -> pure (env, Right val)

  EvaluatorM pf <*> EvaluatorM px = EvaluatorM $ \env -> do
    (env', mf) <- pf env
    (env'', mx) <- px env'
    pure $ (env'', mf <*> mx)

instance Monad EvaluatorM where
  EvaluatorM lhs >>= f = EvaluatorM $ \env -> do
    (env', val) <- lhs env
    case val of
      Left l -> pure (env', Left l)
      Right r -> let EvaluatorM rhs = f r in rhs env'

instance MonadIO EvaluatorM where
  liftIO io = EvaluatorM $ \env -> (,) env <$> Right <$> io
