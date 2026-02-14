// ============================================================================
// COMPREHENSIVE JAVASCRIPT TEST - QuickJS Engine
// ============================================================================

console.log("=== Starting Comprehensive JavaScript Test ===");

// ============================================================================
// 1. BASIC VARIABLES AND DATA TYPES
// ============================================================================
console.log("\n--- 1. Variables and Data Types ---");

let counter = 0;
const MAX_COUNT = 5;
var globalVar = "I'm global";

// Different data types
let stringVar = "Hello World";
let numberVar = 42;
let floatVar = 3.14159;
let booleanVar = true;
let nullVar = null;
let undefinedVar;

console.log(`String: ${stringVar} (type: ${typeof stringVar})`);
console.log(`Number: ${numberVar} (type: ${typeof numberVar})`);
console.log(`Float: ${floatVar} (type: ${typeof floatVar})`);
console.log(`Boolean: ${booleanVar} (type: ${typeof booleanVar})`);
console.log(`Null: ${nullVar} (type: ${typeof nullVar})`);
console.log(`Undefined: ${undefinedVar} (type: ${typeof undefinedVar})`);

// ============================================================================
// 2. CONTROL STRUCTURES
// ============================================================================
console.log("\n--- 2. Control Structures ---");

// While loop
console.log("While loop:");
while (counter < 3) {
    console.log(`  Counter: ${counter}`);
    counter++;
}

// For loop
console.log("For loop:");
for (let i = 0; i < 3; i++) {
    console.log(`  Iteration: ${i}`);
}

// For...in loop (object)
const obj = { a: 1, b: 2, c: 3 };
console.log("For...in loop:");
for (let key in obj) {
    console.log(`  ${key}: ${obj[key]}`);
}

// Switch statement
const day = 2;
console.log("Switch statement:");
switch (day) {
    case 1:
        console.log("  Monday");
        break;
    case 2:
        console.log("  Tuesday");
        break;
    default:
        console.log("  Other day");
}

// If-else chains
const score = 85;
console.log("If-else conditions:");
if (score >= 90) {
    console.log("  Grade: A");
} else if (score >= 80) {
    console.log("  Grade: B");
} else if (score >= 70) {
    console.log("  Grade: C");
} else {
    console.log("  Grade: F");
}

// ============================================================================
// 3. MATHEMATICAL OPERATIONS
// ============================================================================
console.log("\n--- 3. Mathematical Operations ---");

const a = 10;
const b = 3;

console.log(`Basic operations with ${a} and ${b}:`);
console.log(`  Addition: ${a} + ${b} = ${a + b}`);
console.log(`  Subtraction: ${a} - ${b} = ${a - b}`);
console.log(`  Multiplication: ${a} * ${b} = ${a * b}`);
console.log(`  Division: ${a} / ${b} = ${a / b}`);
console.log(`  Modulo: ${a} % ${b} = ${a % b}`);
console.log(`  Exponentiation: ${a} ** ${b} = ${a ** b}`);

// Math object
console.log("Math object functions:");
console.log(`  Math.PI: ${Math.PI}`);
console.log(`  Math.round(4.7): ${Math.round(4.7)}`);
console.log(`  Math.floor(4.7): ${Math.floor(4.7)}`);
console.log(`  Math.ceil(4.3): ${Math.ceil(4.3)}`);
console.log(`  Math.max(1, 5, 3): ${Math.max(1, 5, 3)}`);
console.log(`  Math.min(1, 5, 3): ${Math.min(1, 5, 3)}`);
console.log(`  Math.random(): ${Math.random()}`);
console.log(`  Math.sqrt(16): ${Math.sqrt(16)}`);

// ============================================================================
// 4. FUNCTIONS
// ============================================================================
console.log("\n--- 4. Functions ---");

// Regular function
function greet(name) {
    return `Hello, ${name}!`;
}

// Function expression
const multiply = function(x, y) {
    return x * y;
};

// Arrow function
const square = (n) => n * n;

// Function with default parameters
function introduce(name, age = 25) {
    return `Hi, I'm ${name} and I'm ${age} years old.`;
}

// Rest parameters
function sum(...numbers) {
    return numbers.reduce((total, num) => total + num, 0);
}

console.log(greet("Alice"));
console.log(`Multiply 4 * 7 = ${multiply(4, 7)}`);
console.log(`Square of 8 = ${square(8)}`);
console.log(introduce("Bob"));
console.log(introduce("Charlie", 30));
console.log(`Sum of 1,2,3,4,5 = ${sum(1, 2, 3, 4, 5)}`);

// Closure example
function createCounter() {
    let count = 0;
    return function() {
        return ++count;
    };
}

const myCounter = createCounter();
console.log(`Counter: ${myCounter()}, ${myCounter()}, ${myCounter()}`);

// ============================================================================
// 5. ARRAYS
// ============================================================================
console.log("\n--- 5. Arrays ---");

const fruits = ["apple", "banana", "orange"];
const numbers = [1, 2, 3, 4, 5];

console.log(`Original fruits: [${fruits.join(", ")}]`);
console.log(`Array length: ${fruits.length}`);

// Array methods
fruits.push("grape");
console.log(`After push: [${fruits.join(", ")}]`);

const removed = fruits.pop();
console.log(`After pop (removed ${removed}): [${fruits.join(", ")}]`);

fruits.unshift("mango");
console.log(`After unshift: [${fruits.join(", ")}]`);

// Array iteration
console.log("Array iteration methods:");
numbers.forEach((num, index) => {
    console.log(`  Index ${index}: ${num}`);
});

const doubled = numbers.map(num => num * 2);
console.log(`Doubled numbers: [${doubled.join(", ")}]`);

const evens = numbers.filter(num => num % 2 === 0);
console.log(`Even numbers: [${evens.join(", ")}]`);

const total = numbers.reduce((sum, num) => sum + num, 0);
console.log(`Sum of numbers: ${total}`);

// ============================================================================
// 6. OBJECTS
// ============================================================================
console.log("\n--- 6. Objects ---");

const person = {
    name: "John Doe",
    age: 30,
    city: "New York",
    hobbies: ["reading", "swimming", "coding"],

    // Method
    introduce: function() {
        return `Hi, I'm ${this.name}, ${this.age} years old, from ${this.city}`;
    },

    // Method with arrow function (note: 'this' behaves differently)
    getHobbies: () => {
        return person.hobbies;
    }
};

console.log(person.introduce());
console.log(`Hobbies: [${person.getHobbies().join(", ")}]`);

// Object manipulation
person.email = "john@example.com"; // Add property
person.age = 31; // Modify property
delete person.city; // Delete property

console.log("After modifications:");
for (let key in person) {
    if (typeof person[key] !== 'function') {
        console.log(`  ${key}: ${person[key]}`);
    }
}

// Object.keys, Object.values, Object.entries
const car = { brand: "Toyota", model: "Camry", year: 2020 };
console.log(`Car keys: [${Object.keys(car).join(", ")}]`);
console.log(`Car values: [${Object.values(car).join(", ")}]`);
console.log("Car entries:");
Object.entries(car).forEach(([key, value]) => {
    console.log(`  ${key}: ${value}`);
});

// ============================================================================
// 7. STRING OPERATIONS
// ============================================================================
console.log("\n--- 7. String Operations ---");

const text = "JavaScript is awesome!";
const name = "world";

console.log(`Original text: "${text}"`);
console.log(`Length: ${text.length}`);
console.log(`Uppercase: "${text.toUpperCase()}"`);
console.log(`Lowercase: "${text.toLowerCase()}"`);
console.log(`Substring (0, 10): "${text.substring(0, 10)}"`);
console.log(`Slice (11, -1): "${text.slice(11, -1)}"`);
console.log(`Index of 'Script': ${text.indexOf('Script')}`);
console.log(`Includes 'awesome': ${text.includes('awesome')}`);
console.log(`Replace 'awesome' with 'great': "${text.replace('awesome', 'great')}"`);

// Template literals
console.log(`Template literal: Hello, ${name}! Today is ${new Date().toDateString()}`);

// String splitting and joining
const sentence = "The quick brown fox";
const words = sentence.split(" ");
console.log(`Split into words: [${words.join(", ")}]`);
console.log(`Joined with dashes: "${words.join("-")}"`);

// ============================================================================
// 8. ERROR HANDLING
// ============================================================================
console.log("\n--- 8. Error Handling ---");

// Try-catch-finally
function riskyOperation(value) {
    if (value < 0) {
        throw new Error("Negative values not allowed");
    }
    return Math.sqrt(value);
}

try {
    console.log(`Square root of 16: ${riskyOperation(16)}`);
    console.log(`Square root of -4: ${riskyOperation(-4)}`);
} catch (error) {
    console.log(`Caught error: ${error.message}`);
} finally {
    console.log("Finally block executed");
}

// ============================================================================
// 9. DATE AND TIME
// ============================================================================
console.log("\n--- 9. Date and Time ---");

const now = new Date();
const specificDate = new Date(2024, 0, 15); // Month is 0-indexed

console.log(`Current date: ${now}`);
console.log(`Current timestamp: ${now.getTime()}`);
console.log(`Year: ${now.getFullYear()}`);
console.log(`Month: ${now.getMonth() + 1}`); // +1 because months are 0-indexed
console.log(`Day: ${now.getDate()}`);
console.log(`Hours: ${now.getHours()}`);
console.log(`Minutes: ${now.getMinutes()}`);
console.log(`Seconds: ${now.getSeconds()}`);

console.log(`Formatted date: ${now.toDateString()}`);
console.log(`Formatted time: ${now.toTimeString()}`);
console.log(`ISO string: ${now.toISOString()}`);

// ============================================================================
// 10. REGULAR EXPRESSIONS
// ============================================================================
console.log("\n--- 10. Regular Expressions ---");

const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
const phoneRegex = /^\d{3}-\d{3}-\d{4}$/;

const emails = ["test@example.com", "invalid-email", "user@domain.org"];
const phones = ["123-456-7890", "555-0123", "invalid-phone"];

console.log("Email validation:");
emails.forEach(email => {
    console.log(`  "${email}": ${emailRegex.test(email) ? "Valid" : "Invalid"}`);
});

console.log("Phone validation:");
phones.forEach(phone => {
    console.log(`  "${phone}": ${phoneRegex.test(phone) ? "Valid" : "Invalid"}`);
});

// String replacement with regex
const textWithNumbers = "Call me at 123-456-7890 or 555-0123";
const hiddenNumbers = textWithNumbers.replace(/\d{3}-\d{3}-\d{4}/g, "XXX-XXX-XXXX");
console.log(`Original: ${textWithNumbers}`);
console.log(`Hidden: ${hiddenNumbers}`);

// ============================================================================
// 11. ADVANCED CONCEPTS
// ============================================================================
console.log("\n--- 11. Advanced Concepts ---");

// Destructuring
const colors = ["red", "green", "blue"];
const [first, second, third] = colors;
console.log(`Destructured colors: ${first}, ${second}, ${third}`);

const user = { username: "alice", email: "alice@example.com", role: "admin" };
const { username, email, role } = user;
console.log(`Destructured user: ${username} (${email}) - ${role}`);

// Spread operator
const arr1 = [1, 2, 3];
const arr2 = [4, 5, 6];
const combined = [...arr1, ...arr2];
console.log(`Combined arrays: [${combined.join(", ")}]`);

const original = { a: 1, b: 2 };
const extended = { ...original, c: 3, d: 4 };
console.log(`Extended object:`, extended);

// Template tagged functions
function highlight(strings, ...values) {
    return strings.reduce((result, string, i) => {
        const value = values[i] ? `**${values[i]}**` : '';
        return result + string + value;
    }, '');
}

const highlighted = highlight`Hello ${username}, you have ${role} access!`;
console.log(`Tagged template: ${highlighted}`);

// ============================================================================
// 12. JSON OPERATIONS
// ============================================================================
console.log("\n--- 12. JSON Operations ---");

const data = {
    users: [
        { id: 1, name: "Alice", active: true },
        { id: 2, name: "Bob", active: false },
        { id: 3, name: "Charlie", active: true }
    ],
    timestamp: Date.now()
};

const jsonString = JSON.stringify(data, null, 2);
console.log("JSON stringified:");
console.log(jsonString);

const parsed = JSON.parse(jsonString);
console.log(`Parsed back - Active users: ${parsed.users.filter(u => u.active).length}`);

// ============================================================================
// 13. PERFORMANCE TESTING
// ============================================================================
console.log("\n--- 13. Performance Testing ---");

// Simple performance test
const iterations = 100000;
const startTime = Date.now();

let result = 0;
for (let i = 0; i < iterations; i++) {
    result += Math.sqrt(i) * Math.sin(i);
}

const endTime = Date.now();
console.log(`Performance test: ${iterations} iterations took ${endTime - startTime}ms`);
console.log(`Result: ${result.toFixed(2)}`);

// Memory usage (if available)
if (typeof gc !== 'undefined') {
    console.log("Garbage collection available");
    gc();
    console.log("Garbage collection triggered");
}

// ============================================================================
// CONCLUSION
// ============================================================================
console.log("\n=== JavaScript Test Completed Successfully! ===");
console.log(`Total execution time: ${Date.now() - (startTime - (endTime - startTime))}ms`);
console.log("All major JavaScript features tested and working in QuickJS engine.");
