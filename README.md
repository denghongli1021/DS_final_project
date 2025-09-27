# <資料結構Data Structures 11210CS235101>

This project implements a **simple essay search engine** that supports multiple types of queries. The main functionalities include:

- **Prefix Search** – Finds essays containing words that start with a given prefix.
- **Suffix Search** – Finds essays containing words that end with a given suffix.
- **Exact Word Search** – Finds essays containing an exact word match.
- **Wildcard Search** – Finds essays matching a pattern with wildcards (e.g., `<gr*h>`).
- **Boolean Operators** – Supports query combinations using **AND (`+`)**, **OR (`/`)**, and **Exclude (`-`)** operators.

The engine processes a set of essay `.txt` files and a query file, returning the titles of essays that match the given search conditions. It only considers alphabetic characters and treats uppercase and lowercase letters as equivalent.
