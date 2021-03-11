namespace Knight
{
	public abstract class NonIdempotent : IValue
	{
		public abstract IValue Run();
		public abstract void Dump();

		public override string ToString() => Run().ToString();
		public long ToNumber() => Run().ToNumber();
		public bool ToBoolean() => Run().ToBoolean();
	}
}