namespace Knight
{
	public class KnightException : System.Exception
	{
		public KnightException(string message) : base(message) { }
		public KnightException(string message, System.Exception inner) : base(message, inner) { }
	}

	public class RuntimeException : KnightException
	{
		public RuntimeException(string message) : base(message) { }
		public RuntimeException(string message, System.Exception inner) : base(message, inner) { }
	}

	public class ParseException : KnightException
	{
		public ParseException(string message) : base(message) { }
		public ParseException(string message, System.Exception inner) : base(message, inner) { }
	}
}