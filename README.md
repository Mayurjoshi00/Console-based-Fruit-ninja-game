# 🍉 Fruit Ninja (Console Version in C)

A **console-based Fruit Ninja game** built in **C using the Windows Console API**.
The game features **real-time keyboard input**, **multiple difficulty levels**, **smooth flicker-free rendering**, and a **menu-driven level selection system**.

This project demonstrates low-level game development concepts such as **frame-based updates**, **double buffering**, **edge-based input handling**, and **state management** — all without using any external libraries or game engines.

---

## 🎮 Features

* **Interactive Level Selection Menu**

  * Level 1: Classic gameplay
  * Level 2: Faster falling objects
  * Level 3: Zig-zag moving fruits and bombs
* **Real-time Player Control**

  * Move the blade using arrow keys
* **Dynamic Object Spawning**

  * Random fruits and bombs fall from the top
* **Collision Detection**

  * Slice fruits to gain points
  * Hitting a bomb ends the round
* **Score Tracking**
* **Return-to-Menu System**

  * After each round, players can return to the level menu
* **Smooth Rendering**

  * Uses double buffering to prevent flickering
* **Fixed Console Layout**

  * Locked console size with borders for better visuals

---

## 🕹️ Controls

| Key         | Action                              |
| ----------- | ----------------------------------- |
| ⬅️ ⬆️ ⬇️ ➡️ | Move blade                          |
| ENTER       | Select menu option / return to menu |
| ESC / Q     | Quit game                           |

---

## 🧠 Core Concepts Used

* **GetAsyncKeyState()** for non-blocking keyboard input
* **Edge-based input detection** to prevent repeated key actions
* **Double buffering** using `screen[][]` and `oldScreen[][]`
* **Frame-based timing** for consistent gameplay speed
* **Structure-based object management**
* **Probability-based spawning**
* **Level-based difficulty scaling**

---

## 🧩 Levels Overview

### Level 1 – Classic

* Moderate falling speed
* Balanced fruit and bomb spawn rate

### Level 2 – Speed Rush

* Faster falling objects
* Increased bomb frequency

### Level 3 – Chaos Slice

* Fruits and bombs move in zig-zag patterns
* Highest difficulty and unpredictability

---

## ⚙️ How to Compile & Run

### Using GCC (MinGW)

bash
gcc fruits.c -o fruits.exe
./fruits.exe

> ⚠️ This project is **Windows-only** due to usage of `windows.h`.

---

## 📂 Project Structure

```
fruits/
├── fruits.c
├── fruits.exe
└── README.md
```

---

## 🚀 Future Enhancements

* High score saving using file I/O
* Pause menu
* Power-ups and combo system
* Sound effects
* Lives system instead of instant game over
* Cross-platform support (Linux/macOS)

---

## 📌 Conclusion

This project showcases how a complete interactive game can be built using **pure C and basic OS-level APIs**.
It focuses on **performance, correctness, and clean logic**, making it a great learning project for understanding how real-time systems work at a low level.

---

If you want, I can also:

* shorten this for a **minimal README**
* add **badges**
* write a **project summary for resume**
* generate a **LICENSE**
* suggest **commit messages**

Just tell me 👍

