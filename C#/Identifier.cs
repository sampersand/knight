using System.Collections.Generic;

namespace Knight
{
    public class Identifier : NonIdempotent
    {
		private static IDictionary<string, IValue> ENV = new Dictionary<string, IValue>();

		private string _name;

		public Identifier(string name) => _name = name;

		public override IValue Parse(ref string stream) {
			string ident = "";

			while (char.IsLower(stream[0]) || stream[0] == '_') {
				ident += stream[0];
				stream = stream.Substring(1);
			}

			return ident == "" ? null : new Identifier(ident);
		}

		// note: Throws `KeyNotFoundException` if `_name` doesn't exist.
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
