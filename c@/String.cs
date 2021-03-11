using System;
using Knight.Ops;

namespace Knight
{
	public class String : Literal<string>, IAdd, IMul, IComparable<IValue>
	{
		public String(string data) : base(data) {}

		public static String Parse(Stream stream) {
			if (!stream.StartsWith('\'', '\"'))
				return null;
			
			char quote = stream.Take();
			var start = stream.Source;

			string data = stream.TakeWhile(c => c != quote);
			if (stream.IsEmpty()) {
				throw new RuntimeException($"unterminated string, starting at {start}");
			}

			stream.Take(); // remove trailling quote.

			return new String(data ?? "");
		}

		public override void Dump() => Console.Write($"String({_data})");

		public override bool ToBoolean() => _data != "";
		public override long ToNumber() {
			long ret = 0;
			var str = _data.TrimStart();

			if (str == "")
				return 0;

			var isNegative = str[0] == '-';

			if (str[0] == '-' || str[0] == '+')
				str = str.Substring(1);

			for (; str != "" && char.IsDigit(str[0]); str = str.Substring(1))
				ret = ret * 10 + (str[0] - '0');

			if (isNegative)
				ret *= -1;

			return ret;
		}

		public IValue Add( IValue rhs) => new String(_data + rhs);
		public IValue Mul(IValue rhs) {
			var s = "";
			var amnt = rhs.ToNumber();

			for (long i = 0; i < amnt; ++i)
				s += _data;
			return new String(s);
		}

		public int CompareTo(IValue obj) => string.Compare(_data, obj.ToString(), StringComparison.Ordinal);
	}
}