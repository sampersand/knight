using System;
using System.IO;

namespace Knight
{
	public class Kn
	{

		internal static IValue Parse(ref string stream) {
			IValue value;

			while (stream != "") {
				switch (stream[0]) {
					case '#':
						do {
							stream = stream.Substring(1);
						} while (stream != "" && stream[0] != '\n');
						break;

					case '(':
					case ')':
					case '[':
					case ']':
					case '{':
					case '}':
					case ':':
						stream = stream.Substring(1);
						break;
					default:
						if (char.IsWhiteSpace(stream[0]))
							goto case ':';
						goto foo;
				}
			}
			foo:

			if (stream == "")
				return null;

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

		public static IValue Run(string stream) {
			if (stream == "") {
				throw new ParseException("nothing to parse.");
			}

			IValue value = Parse(ref stream);
			
			if (value == null) {
				throw new ParseException($"Unknown token start '{stream[0]}'.");
			} else {
				return value.Run();
			}
		}

		static int Main(string[] args) {
			if (args.Length != 2 || args[0] != "-e" && args[0] != "-f") {
				Console.Error.WriteLine("usage: {0} (-e 'program' | -f file)", Environment.GetCommandLineArgs()[0]);
				return 1;
			}

			try {
				Run(args[0] == "-e" ? args[1] : File.ReadAllText(args[1]));
				return 0;
			} catch (KnightException err) {
				Console.Error.WriteLine("invalid program: {0}", err.Message);
				return 1;
			}
		}
	}
}