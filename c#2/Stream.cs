using System;
using System.Linq;

namespace Knight
{
	public class Stream
	{
		public string Source { get; private set; }
		public delegate bool Condition(char c);

		public Stream(string source) => Source = source;

		public bool IsEmpty() => Source == "";

		public bool StartsWith(Condition cond) => !IsEmpty() && cond(Source[0]);
		public bool StartsWith(params char[] chars) => StartsWith(chars.Contains);

		public void StripKeyword() => TakeWhile(c => char.IsUpper(c) || c == '_');

		public string TakeWhileIfStartsWith(char chr, Condition body) => TakeWhileIfStartsWith(c => c == chr, body);
		public string TakeWhileIfStartsWith(char[] chars, Condition body) => TakeWhileIfStartsWith(chars.Contains, body);
		public string TakeWhileIfStartsWith(Condition initial, Condition body = null) {
			if (!initial(Source[0])) {
				return null;
			} else {
				return Take() + (TakeWhile(body ?? initial) ?? "");
			}
		}

		public char Take() {
			char c = Source[0];
			Source = Source.Substring(1);
			return c;
		}

		public string TakeWhile(Condition condition) {
			var ret = "";

			while (!IsEmpty() && condition(Source[0])) {
				ret += Take();
			}

			return ret == "" ? null : ret;
		}
	}
}