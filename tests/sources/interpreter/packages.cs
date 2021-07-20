function test1() {
	echo("hey, i'm test");
	return 500;
}

package test1_1 {
	function test1() {
		echo("package test1 1");
		return Parent::test1() / 2;
	}
};

package test1_2{
	function test1() {
		echo("package test1 2");
		return Parent::test1() / 3;
	}
};

package test1_3 {
	function test1() {
		echo("package test1 3");
		return Parent::test1() / 4;
	}
};

package test1_4 {
	function test1() {
		echo("package test1 4");
		return Parent::test1() / 5;
	}
};

echo(test1());

function test2() {
	echo("hey, i'm test");
	return 500;
}

package test2_1 {
	function test2() {
		%result = Parent::test2() / 2;
		echo("package test2 1");
		return %result;
	}
};

package test2_2{
	function test2() {
		%result = Parent::test2() / 3;
		echo("package test2 2");
		return %result;
	}
};

package test2_3 {
	function test2() {
		%result = Parent::test2() / 4;
		echo("package test2 3");
		return %result;
	}
};

package test2_4 {
	function test2() {
		%result = Parent::test2() / 5;
		echo("package test2 4");
		return %result;
	}
};

echo(test2());