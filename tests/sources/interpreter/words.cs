%test[0] = "hey" SPC "how" SPC "are" SPC "you" SPC "doing" SPC "?";
%test[1] = "hey" TAB "how" TAB "are" TAB "you" TAB "doing" TAB "?";
%test[2] = "hey" NL "how" NL "are" NL "you" NL "doing" NL "?";

// first word tests
for(%i = 0; %i < 3; %i++) {
	echo(firstWord(%test[%i]));
}

// rest word tests
for(%i = 0; %i < 3; %i++) {
	echo(restWords(%test[%i]));
}

// get word tests part 1
for(%i = 0; %i < 3; %i++) {
	for(%j = -1; %j < getWordCount(%test[0]) + 1; %j++) {
		echo(getWord(%test[%i], %j));
	}
}

// get words tests part 2
for(%i = 0; %i < 3; %i++) {
	for(%j = -1; %j < getWordCount(%test[0]) + 1; %j++) {
		echo(getWords(%test[%i], %j, %j + 1));
		echo(getWords(%test[%i], %j));
	}
}

// remove word tests
for(%i = 0; %i < 3; %i++) {
	for(%j = -2; %j < getWordCount(%test[0]) + 2; %j++) {
		echo(removeWord(%test[%i], %j));
	}
}

// set word tests
for(%i = 0; %i < 3; %i++) {
	for(%j = -2; %j < getWordCount(%test[0]) + 2; %j++) {
		echo(setWord(%test[%i], %j, "frog"));
	}
}
