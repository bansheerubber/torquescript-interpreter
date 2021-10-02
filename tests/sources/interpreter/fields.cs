%test[0] = "hey" SPC "how" SPC "are" SPC "you" SPC "doing" SPC "?";
%test[1] = "hey" TAB "how" TAB "are" TAB "you" TAB "doing" TAB "?";
%test[2] = "hey" NL "how" NL "are" NL "you" NL "doing" NL "?";

// first word tests
for(%i = 0; %i < 3; %i++) {
	echo(firstField(%test[%i]));
}

// rest word tests
for(%i = 0; %i < 3; %i++) {
	echo(restFields(%test[%i]));
}

// get word tests part 1
for(%i = 0; %i < 3; %i++) {
	for(%j = -1; %j < getFieldCount(%test[0]) + 1; %j++) {
		echo(getField(%test[%i], %j));
	}
}

// get words tests part 2
for(%i = 0; %i < 3; %i++) {
	for(%j = -1; %j < getFieldCount(%test[0]) + 1; %j++) {
		echo(getFields(%test[%i], %j, %j + 1));
		echo(getFields(%test[%i], %j));
	}
}

// remove word tests
for(%i = 0; %i < 3; %i++) {
	for(%j = -2; %j < getFieldCount(%test[0]) + 2; %j++) {
		echo(removeField(%test[%i], %j));
	}
}

// set word tests
for(%i = 0; %i < 3; %i++) {
	for(%j = -2; %j < getFieldCount(%test[0]) + 2; %j++) {
		echo(setField(%test[%i], %j, "frog"));
	}
}
