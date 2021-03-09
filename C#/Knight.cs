using System;
using System.IO;

namespace Knight
{
	public class Kn
	{
		public static IValue Run(string stream) {
			IValue value;
			
			if (stream == "" || (value = IValue.Parse(ref stream)) == null) {
				throw new ParseException($"Unknown token start '{stream[0]}'.");
			} else {
				return value;
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
				Console.Error.WriteLine("invalid program: {0}", err);
				return 1;
			}
		}
	}
}