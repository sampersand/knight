from knight.stream import Stream
from knight.error import Error, RunError, ParseError
from knight.value import Value

from knight.literal import Literal
from knight.null import Null
from knight.boolean import Boolean
from knight.number import Number
from knight.string import String
from knight.identifier import Identifier
from knight.function import Function

def run(stream: str) -> Value:
	if value := Value.parse(Stream(stream)):
		return value.run()
	else:
		raise ParseError('Nothing to parse.')

