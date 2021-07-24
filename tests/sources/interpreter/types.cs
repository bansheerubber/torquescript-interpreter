function ScriptObject::test(%this) {
	echo(%this.hey);
}

%obj[0] = new ScriptObject() {
	hey = "how are you doing";
};

%obj[1] = new ScriptObject() {
	hey = "not very well";
};

%obj[2] = new ScriptObject() {
	hey = "superb";
};

%obj[3] = -1;

%objNumber[0] = 0;
%objNumber[1] = 1;
%objNumber[2] = 2;
%objNumber[3] = 3;

%objString[0] = "0";
%objString[1] = "1";
%objString[2] = "2";
%objString[3] = "3";

for(%i = 0; %i < 4; %i++) {
	echo(%obj[%i].hey);
	echo(%objNumber[%i].hey);
	echo(%objString[%i].hey);

	%obj[%i].test();
	%objNumber[%i].test();
	%objString[%i].test();

	echo(-%obj[%i]);
	echo(-%objNumber[%i]);
	echo(-%objString[%i]);
}