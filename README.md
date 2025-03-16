# Global Agenda SDK experiments

Merely trying out random stuff, I have no idea what I'm doing.

I do however want it to be more a community effort rather than 

Stuff I'm looking for:

- loading into any map & exploring with noclip
- see if any server-side code exists and can be triggered via the SDK
- implement a server? sounds impossible right now, don't get your hopes up
- really anything that might allow anyone to play the game, even if in limited ways

Stuff I'm NOT looking for:

- aimbots, wallhacks, speedhacks, RCE exploits etc., anything that would be considered cheating or just evil

Disclaimer: I am not affiliated with the game or the developers in any way. I'm making no profit from this and I never will.

## Building

There is a github action that should build a `version.dll` file.

This dll should go into the same directory where `GlobalAgenda.exe` is located.

You also need to copy the original `version.dll` from your system and put it next to the `GlobalAgenda.exe` file as `version_orig.dll`.

I have no clue if that's enough. Maybe you need to inject it manually using some tool, I don't know, you tell me. Even better - if it doesn't work, feel free to fix it yourself and send a pull request.

On linux you need to set the `WINEDLLOVERRIDES="version=n,b"` environment variable while running the game.

