%var[0] = 5.135;
%var[1] = "hey there";
%var[2] = "100.05";
%var[3] = new ScriptObject();

for(%i = 0; %i < 4; %i++) {
	for(%j = 0; %j < 4; %j++) {
		echo(%var[%i] @ %var[%j]);
		echo(%var[%i] + %var[%j]);
		echo("");
	}	
}