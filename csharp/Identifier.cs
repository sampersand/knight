using System.Collections.Generic;
using System;

namespace Knight
{
    public class Identifier : NonIdempotent, IValue
    {
		private static IDictionary<string, IValue> ENV = new Dictionary<string, IValue>();

		private string _name;

		public Identifier(string name) => _name = name;

		public static Identifier Parse(Stream stream) {
			bool isLower(char c) => char.IsLower(c) || c == '_';

			var contents = stream.TakeWhileIfStartsWith(isLower, c => isLower(c) || char.IsDigit(c));
			
			return contents == null ? null : new Identifier(contents);
		}

		public override void Dump() => Console.Write($"Identifier({_name})");

		public override IValue Run() {
			IValue result;

			if (ENV.TryGetValue(_name, out result)){
				return result;
			} else {
				throw new RuntimeException($"Unknown identifier '{_name}'.");
			}
		}

		public void Assign(IValue value) => ENV[_name] = value;
    }
}
