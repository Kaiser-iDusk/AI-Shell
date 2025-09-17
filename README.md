<h2>Shell with Gemini Integration</h2>
<h3>About</h3>
<p>This is a shell or a terminal where if you forget some Linux commands, you can directly ask the terminal itself with context of the files already in the current directory. Powered by Gemini-Flash-2.0, this shell can answer queries by just pressing the "]" key and typing in natural english.</p>
<h3>Screenshots</h3>
<img width="745" height="435" alt="1" src="https://github.com/user-attachments/assets/3dd3b93e-5ef7-441d-baca-6b888976a14a" />

<img width="745" height="197" alt="2" src="https://github.com/user-attachments/assets/112e3a90-3120-4a06-8ecf-82f3b44937d4" />

<img width="1618" height="198" alt="4" src="https://github.com/user-attachments/assets/31ff1d68-8db1-4bc0-a45e-5e5b74ec81fc" />

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
