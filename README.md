# Wordle
A C++ application to help solve the word puzzle at https://www.powerlanguage.co.uk/wordle/ or https://qntm.org/files/wordle/index.html


WordleClient is the interactive excutable to help play the game. It keeps suggesting words for the game and based on the status of these words, it keeps updating its set of possible words.

Please note that:

1) If WordleClient is run without any arguments, default word-size is 5 and default display limit (maximum number of words shown on the terminal) is 10
2) After every suggested word is entered, it also expects the status of previous word, so that it can update the possible set of words
   - Status string is made of only:
      * 'b' for Black color
      * 'g' for Green color
      * 'y' for Yellow color
      
      
      e.g. For 'apple', if the sequence of colors shown on Wordle/Absurdle is [Black, Black, Green, Yellow, Black], then status string should be 'bbgyb'
      
  3) It is also possible that the correct ans is not in the dictionary used by the program, then program returns.
  4) There is also a main.cpp but this is just used to run tests, wordle_client_main.cpp is the file which should be built/run to play the game!


# How to build
There is a CMakeLists.txt file to help build this project

# How to run
```
user$ ./WordleClient
Welcome! word size is: [5], display limit is: [10]
There are 12478 possible words, try one of these: 
amens
aways
cimar
doilt
draps
gruel
lawny
meiny
sheep
sotol
Enter the selected word:
```

You can also provide your own arguments:
```
user$ ./WordleClient 5 8
Welcome! word size is: [5], display limit is: [8]
There are 12478 possible words, try one of these: 
abler
anoas
copal
hiant
soots
uptie
varan
wests
Enter the selected word:
```

# Example run
Following is an example run for the puzzle on https://www.powerlanguage.co.uk/wordle/ on 12th Jan, 2022
```
user$ ./WordleClient 5 5
Welcome! word size is: [5], display limit is: [5]
There are 12478 possible words, try one of these: 
crees
fever
misos
refed
snips
Enter the selected word: snips
Enter the status of previous word: bbbbb
There are 3014 possible words, try one of these: 
bortz
corbe
crwth
dotal
gamba
Enter the selected word: corbe
Enter the status of previous word: byybb
There are 73 possible words, try one of these: 
argol
froth
grout
growl
rumor
Enter the selected word: growl
Enter the status of previous word: byybb
There are 16 possible words, try one of these: 
ottar
ovary
ratoo
razoo
rumor
Enter the selected word: rumor
Enter the status of previous word: bbbgg
Only following 2 possible words remaining: 
favor
taxor
Enter the selected word: favor
Enter the status of previous word: ggggg
Congratulations! you eventually found the word!

```
