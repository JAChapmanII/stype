import std.io;
import std.string;
import core.type;

type string = std.string;
var out = std.io.stdout;

type Cat {
	func speak() -> string;
}

type Dog {
	func speak() -> string;
}

func Cat.speak() -> string {
	return "meow";
}

func Dog.speak() -> string {
	return "woof";
}

type Animal = Dog >< Cat;

func pet(Animal animal) -> void {
	out.print("petting an %s", core.type.getTypeName(animal));
	out.print("result: %s", animal.speak());
}

func main() -> void {
	out.print("Hello, World!\n");

	Cat cat = new Cat();
	out.print(cat.speak());

	Dog dog = new Dog();
	out.print(dog.speak());
}

