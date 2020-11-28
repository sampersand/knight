class Sq::Array is Sq::Value is export {
	has @.values;

	#| Create a new Array with the given argument array.
	method new(::?CLASS:U: *@values --> ::?CLASS:D) {
		self.bless(:@values)
	}


	#| Converts this Array to a String in the format `[arg1, arg2, ...]`.
	method to-string(--> Sq::String:D) {
		Sq::String.new("[" ~ @!values>>.to-string.join(", ") ~ "]")
	}

	#| Converts this Array to a Boolean, where empty arrays are false.
	method to-bool(--> Sq::Boolean:D) {
		Sq::Boolean.new(?@!values)
	}

	#| Simply returns itself.
	method to-array(--> Sq::Array:D) {
		self
	}

	#| Gets the length of this Array.
	method length(--> Sq::Number:D) {
		Sq::Number.new(@!values.elems)
	}

	#| Replaces all `Nil`s with `Null`s
	method cleanup-nil {
		for @!values <-> $value {
			$value //= Sq::Null.new
		}
	}

	#| Sets the value at the given location of the array.
	method set-index(Sq::Value:D $index, Sq::Value:D $value) {
		@!values[$index.to-number.value] = $value
	}
	
	#| Inserts an element into the given location of the Array.
	method insert(Sq::Value:D $index, Sq::Value:D $value) {
		@!values.splice($index.to-number.value, 0, $value)
	}

	#| Deletes the element at the given index in the Array.
	method delete(Sq::Value:D $index --> Sq::Value:D) {
		my $idx = $index.to-number.value;

		if @!values[$idx]:exists {
			@!values.splice($idx, 1)[0]
		} else {
			Sq::Null.new
		}
	}

	#| Returns the index of an element, or `Null` if it odesn't exist.
	method index-of(Sq::Value:D $element --> Sq::Number:D | Sq::Null:D) {
		with my $index = @!values.first(*.eql($element)):k {
			Sq::Number.new($index)
		} else {
			Sq::Null.new
		}
	}

	#| Gets an element out of the array.
	method index(Sq::Value:D $index --> Sq::Value:D) {
		@!values[$index.to-number.value] // Sq::Null.new
	}

	#| Adds another Array to this one by concating it.
	method add(Sq::Value:D $other --> Sq::Array:D) {
		Sq::Array.new(|@!values, |$other.values)
	}

	#| Duplicates this array by the amount of elements given.
	method mul(Sq::Value:D $times --> Sq::Array:D) {
		# possible footgun: this does not clone the current array.
		# try doing `@!values.clone` if you want to copy it.

		Sq::Array.new(|(@!values xx $times.to-number.value))
	}

	#| Compares two arrays by returning `-1`, or `1` for the first value that doesn't compare,
	#| or `0` if they're equal.
	method cmp(Sq::Value:D $other --> Sq::Number:D) {
		my @other = $other.to-array.values;

		if @!values == @other {
			for zip(@!values, @other, with => {$^a.cmp: $^b}) -> $cmp {
				if $cmp != 0 {
					return Sq::Number.new($cmp)
				}
			}

			Sq::Number.new(0)
		} else {
			Sq::Number.new(@!values <=> @other)
		}
	}
	
	#| Checks to see if two arrays are equal.
	method eql(Sq::Value:D $other --> Sq::Boolean:D) {
		Sq::Boolean.new(self.cmp(other).value == 0)
	}
}
