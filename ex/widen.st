import std.io;
import std.string;
import core.type;

type string = std.string;
var out = std.io.stdout;

type Cat {
	func speak() -> string { return "meow"; }
}

type Dog {
	func speak() -> string { return "woof"; }
}

type Bird {
	func speak() -> string { return "chirp"; }
}

type Animal = Dog >< Cat;

func pet(Animal animal) -> void {
	out.println("petting an %s", core.type.getTypeName(animal));
	out.println("result: %s", animal.speak());
}

func main() -> void {
	out.print("Hello, World!\n");

	Cat cat = new Cat();
	pet(cat);

	Dog dog = new Dog();
	pet(dog);

	Bird bird = new Bird();
	pet(bird);
}

