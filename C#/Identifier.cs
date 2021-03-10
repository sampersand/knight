using System.Collections.Generic;
using System;

namespace Knight
{
    public class Identifier : NonIdempotent
    {
		private static IDictionary<string, IValue> ENV = new Dictionary<string, IValue>();

		private string _name;

		public Identifier(string name) => _name = name;

		public static IValue Parse(ref string stream) {
			if (!char.IsLower(stream[0]) && stream[0] != '_') {
				return null;
			}

			string ident = "";

			do {
			
				ident += stream[0];
				stream = stream.Substring(1);
			} while (stream != "" && (char.IsDigit(stream[0]) || char.IsLower(stream[0]) || stream[0] == '_'));

			return new Identifier(ident);
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
