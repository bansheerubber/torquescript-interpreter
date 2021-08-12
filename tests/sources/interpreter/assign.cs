%values[0] = 51;
%values[1] = "51";
%values[2] = new ScriptObject();

$values[0] = 51;
$values[1] = "51";
$values[2] = new ScriptObject();

%test = new ScriptObject();
%test.values[0] = 51;
%test.values[1] = "51";
%test.values[2] = new ScriptObject();

for(%i = 0; %i < 3; %i++) {
	%values[%i] += 2;
	%values[%i] += "5";
	%values[%i] += %test;

	$values[%i] += 2;
	$values[%i] += "5";
	$values[%i] += %test;

	%test.values[%i] += 2;
	%test.values[%i] += "5";
	%test.values[%i] += %test;

	echo(%values[%i]);
	echo($values[%i]);
	echo(%test.values[%i]);
	echo("");
}