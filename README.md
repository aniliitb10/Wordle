# Wordle
To help solve the word puzzle at https://www.powerlanguage.co.uk/wordle/ or https://qntm.org/files/wordle/index.html


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
