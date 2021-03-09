namespace Knight
{
	public interface IValue
	{

		static IValue Parse(ref string stream) {
			IValue value;

			if ((value = Number.Parse(ref stream)) != null) 
				return value;
			if ((value = Boolean.Parse(ref stream)) != null)
				return value;
			if ((value = String.Parse(ref stream)) != null)
				return value;
			if ((value = Null.Parse(ref stream)) != null)
				return value;
			if ((value = Identifier.Parse(ref stream)) != null)
				return value;
			if ((value = Function.Parse(ref stream)) != null)
				return value;
			return null;
		}

		IValue Run();
		void Dump();

		string ToString();
		bool ToBoolean();
		long ToNumber();
	}
}

