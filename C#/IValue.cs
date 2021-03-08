namespace Knight
{
	public interface IValue {
		IValue Run();
		IValue Parse(ref string stream);

		string ToString();
		bool ToBoolean();
		long ToNumber();
	}
}

