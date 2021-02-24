from __future__ import annotations

from knight import Stream
from typing import Union

class Value():
	TYPES = []

	@staticmethod
	def parse(stream: Stream) -> Union[None, Value]:
		stream.strip()

		for cls in TYPES:
			value = cls.parse(stream)

			if value != None:
				return value

	def run(self) -> Value:
		raise NotImplemented

	# who the heck deprecates something as cool as `__cmp__`
	def cmp(self, rhs: Value) -> int:
		"""
		Compare `self` to `rhs`, returning a negative, `0`, or positive number depending on whether
		`self` is less than, equal to, or greater than `rhs`.
		"""
		raise NotImplemented

	def __int__(self):
		""" Converts this class to an integer """
		return int(self.run())

	def __str__(self):
		""" Converts this class to a string """
		return str(self.run())

	def __bool__(self):
		""" Converts this class to a boolean """
		return bool(self.run())

	# who the heck deprecates something as cool as `__cmp__`
	def cmp(self, rhs: Value) -> int:
		"""
		Compare `self` to `rhs`, returning a negative, `0`, or positive number depending on whether
		`self` is less than, equal to, or greater than `rhs`.
		"""
		raise NotImplemented

	def __lt__(self, rhs: Value):
		""" Checks to see if `self` is less than `rhs` """
		return self.cmp(rhs) < 0

	def __gt__(self, rhs: Value):
		""" Checks to see if `self` is greater than `rhs` """
		return self.cmp(rhs) > 0

"""

	/**
	 * Converts this value to a int.
	 *
	 * @return int
	 **/
	abstract public function toInt(): int;

	/**
	 * Converts this value to a bool.
	 *
	 * @return bool
	 **/
	abstract public function toBool(): bool;

	/**
	 * Gets a string representation of this class, for debugging purposes.
	 *
	 * @return string
	 **/
	abstract public function dump(): string;

	/**
	 * Checks to see if `$this` is equal to `$value`.
	 *
	 * @return bool
	 **/
	abstract public function eql(Value $value): bool;

	/**
	 * Executes this Value.
	 *
	 * By default, the return value is simply `$this`.
	 *
	 * @return Value The result of running this value.
	 **/
	public function run(): Value
	{
		return $this;
	}

	/**
	 * Checks to see if this value is less than the other.
	 *
	 * This calls the `cmp` and then checks to make sure the value is less than zero.
	 *
	 * @param Value $rhs The value to test against.
	 * @return bool Returns `true` if `$this` is less than `$rhs`.
	 */
	public function lth(Value $rhs): bool
	{
		return $this->cmp($rhs) < 0;
	}

	/**
	 * Checks to see if this value is greater than the other.
	 *
	 * This calls the `cmp` and then checks to make sure the value is greater than zero.
	 *
	 * @param Value $rhs The value to test against.
	 * @return bool Returns `true` if `$this` is greater than `$rhs`.
	 */
	public function gth(Value $rhs): bool
	{
		return $this->cmp($rhs) > 0;
	}
}
"""
'''
