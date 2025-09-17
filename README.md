<h2>Shell with Gemini Integration</h2>
<h3>About</h3>
<p>This is a shell or a terminal where if you forget some Linux commands, you can directly ask the terminal itself with context of the files already in the current directory. Powered by Gemini-Flash-2.0, this shell can answer queries by just pressing the "]" key and typing in natural english.</p>
<h3>Screenshots</h3>

<h3>How to Use</h3>
<p>Compile the code using:</p>
```bash
g++ -Iincludes -std=c++17 *.cpp -o <some_name> -lcurl
```
<p>Also make sure to create a `.env` file and write the following:</p>
```
GEMINI_API_KEY={YOUR_API_KEY}
```
<p>You are all set now, just run the compiled binary.</p>
