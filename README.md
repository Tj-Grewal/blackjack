# Blackjack (simple console game)

A small C++ console implementation of a single-player Blackjack (21) game vs. the dealer.

## What it is
- Player plays rounds against a dealer using standard Blackjack rules: try to get as close to 21 without going over.
- The dealer follows simple rules (hits until a threshold, then stands).
- Input-driven: run the program and follow prompts to Hit or Stand.

Follow on-screen prompts. Typical flow:
- Game deals initial cards and prints your and dealer's visible cards
- You choose Hit (draw) or Stand
- After you stand or bust, dealer plays and result is shown

## Notable features
- Colors and visuals: the game uses ANSI colors for headings, advice and suits where supported.
- Unicode suits: the program will attempt to enable UTF-8 and print ♠ ♥ ♦ ♣ on terminals that support it; it falls back to ASCII (S/H/D/C) automatically on consoles that don't.
- Soft/hard totals: when your hand contains an Ace, the program prints both the soft and hard totals (e.g. "Totals: 14 (soft), 4 (hard)").
- Current score: after each deal/hit the game prints an explicit "Current score: X" line so you always see which total is being used.
- Ace revaluation message: when an Ace switches from 11 to 1 to avoid busting, the game prints "(Ace counted as 1 to avoid bust)".
- Advisor: the built-in advisor prints colorized guidance (HIT/ STAND) based on the dealer's up-card.

## Controls / Input
- Enter `h` or `H` to Hit (draw another card)
- Enter `s` or `S` to Stand
- Enter `y` to play another hand when prompted
- Program handles invalid input by re-prompting

If your terminal doesn't show Unicode suits correctly try PowerShell 7, Windows Terminal, or use the ASCII fallback (the program auto-detects support).

## Example
```
+Player+: [3♣]  [A♥]
*Dealer*: [9♥] [??]

	Totals: 14 (soft), 4 (hard)
	Current score: 14
	ADVISOR observation: The dealer upcard is good.
	Advice: You should HIT until your score is at least 17

Dealing to player:
Enter h to hit or s to stand: h
	(Ace counted as 1 to avoid bust)
	Totals: 12 (soft), 12 (hard)
	Current score: 12
```

## Notes
- The program attempts to enable ANSI colours and UTF-8 on Windows. If you see escape codes instead of colours, switch to a modern terminal.
- The advisor is a simple heuristic and intended as a learning aid.
