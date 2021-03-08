namespace Knight
{
	public abstract class NonIdempotent : IValue
	{
		public abstract IValue Run();
		public abstract IValue Parse(ref string stream);

		public override string ToString() => Run().ToString();
		public long ToNumber() => Run().ToNumber();
		public bool ToBoolean() => Run().ToBoolean();
	}
}