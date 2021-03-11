namespace Knight
{
	public interface IValue
	{
		IValue Run();
		void Dump();

		string ToString();
		bool ToBoolean();
		long ToNumber();
	}
}

