# Health Resource Chat Bot

This project is aimed at developing a Mental Health AI chat bot to help keep patients company is the hospital, while also being able to provide patients with mental health resouces. 

This guide follows the steps from the "Real Python" tutorial [Building a chat bot with python](https://realpython.com/build-a-chatbot-python-chatterbot/). 




### Contents

1. [Chat Bot Structure](#1-chat-bot-structure)
2. [Creating your Virtual Environment](#2-creating-your-virtual-environment)
3. [Preparing chatterbot Library](#3-preparing-chatterbot-library)
4. [Chat Bot Training](#4-chat-bot-training)
    * [List Trainer](#list-trainer)
    * [Corpus Trainer](#corpus-trainer)
    * [Format Training Data](#format-training-data)
    * [Untraining](#untraining)
5. [Using The Chat Bot](#5-using-the-chat-bot)

# 1. Chat Bot Structure

```bot.py``` is the main file that launches the chatbot. This script does 3 main things to make the chat bot work:
1. Training the chat bot.
2. Reding user input.
3. Replying to user with a line from training data.

The chat bot is trained using conversations formatted into 'input' and 'response'. It then uses these cases to base its answers off, such that if a similar input is made to that in its training data, it will respond with the similar response.

The chat bot runs on an "while loop", prompting a user input, reading the input, and providing a response. If the user input contains one of the pre-defined end conditions (such as 'quit', 'exit', 'goodbye', 'bye') the loop will end, closing the chat bot. Otherwise, the chatbot will copare the user input to its traing data and output the response of the trained input most similar to that of the user.


# 2. Creating Your Virtual Environment
Virtual environments are useed in software development since they managing packages, dependencies, and versions to avoid conflicts between other package versions. Conflicting package versions can cause these packages to be uninstalled to allow for the others to run properly, causing problems for other coding files later on.

First create a folder to store all your chatbot files and virtual environment in. Open that folder in VS code (or another source code editor) and open your terminal.<br><br>
Create your virtual environment named "chatbotenv" using:
```
PS> python -m venv chatbotenv
```
Activate your virtual environment using:
```
PS> chatbotenv\Scripts\activate
```
You should now see the virtual environment name in brackets in your terminal. 

# 3. Preparing chatterbot Library
Install the chatterbot library using:
```
(chatbotenv) PS> python -m pip install chatterbot==1.0.4 pytz
```

Since some of the libraries are out of data, some manual changes need to be made in the files. When you first run ```bot.py``` some of these errors will pop up.

Use the following table to edit them by selecting the link in the terminal to where the problem is occuring and make the following changes.

Error Message |File Path| Line #  | Wrong | Right |
--------------|---------|---------| ------|-------|
```GeneralAttributeError: module 'time' has no attribute 'clock'``` |chatbotenv\Lib\site-packages\sqlalchemy\util\compat.py|264   |```time_func = time.clock``` | ```time_func = time.perf_counter```
```AttributeError: module 'collections' has no attribute 'Hashable'``` | chatbotenv\Lib\site-packages\yaml\constructor.py | 8,9 |```import collections, datetime, base64, binascii, re, sys, types``` | ```import datetime, base64, binascii, re, sys, types, collections, collections.abc``` <br><br> ```collections.Hashable = collections.abc.Hashable```

Once these changes are complete the only error you will get is that your training file does not exist...yet! that is your next step after learning how this chat bot script works.



# 4. Chat Bot Training
This chat bot is a Natural Language Procesing (NLP) chat bot. This type of chat bot needs to be provided with data to train off of if you want it to reply with useful responses. 

Each time ```bot.py``` is run, the training procedures are re-done.

We are using the chatterbot python library to make our bot which can be trained in many ways. The two main ways we will be using are the ```List_trainer``` and the ```Corpus_trainer```. These allow for you to train your chat bot with personalized information in the form of 'input' and 'response'

### List Trainer
One way it can be trained is with the ```List_trainer```. This takes two arguments, a user input and the response which should be given. For example:

```
    List_trainer.train([
    "Hi",
    "Hello there! What can I help you with?",
    ])
```
This instructs the chat bot to resond to "Hi" with "Hello there! What can I help you with?"

### Corpus Trainer
A more efficent way to train the chat bot with large amouns of information is with the ```Corpus_trainer```. This fucntion takes a ```.yml``` file filled with user input cases and responses, and trains the bot on them all. 

There exists pre-made files containing traing data in the chatterbot library (chatterbot_corpus\data), but you can also make your own training files using the same format as seen below.

```
categories:
- Health Companion Training
conversations:
- - User input
  - Chat bot response
- - I am not feeling well today.
  - I am sorry to hear that, would you like to tell me more about how you are feeling?
```
 These files can be implemented into your chat bot by runinng the ```Corpus_trainer``` function where each element in the list below is the path to the specific ```.yml``` file.

```
Corpus_trainer.train(
    "chatterbot.corpus.english.emotion",
    "chatterbot.corpus.english.conversations",
    "chatterbot.corpus.english.greetings",
    "chatterbot.corpus.custom"
)
```



### Format Training Data
Another way to gather data to train the chat bot is to import data from external sources in other forms. 

An example of this is done with [GPT generated therapist transcripts](https://www.kaggle.com/datasets/thedevastator/synthetic-therapy-conversations-dataset). Here, a extensive ```.csv``` file with user inputs and responses can be formatted, using a script, to the exact format needed for the corpus trainer.

Be careful when importing different files with conversational data since some special characters have different meanings when in a ```.yml``` file. It can be usefule to write a line that removes these charcters from your ```.csv``` file or removes specific phrases/ text that you do not want included in the chat bot response:
```
import pandas as pd

# Read the CSV file
df = pd.read_csv('your_file.csv')

# Define the phrases to remove
phrases_to_remove = ['&', '@', 'another phrase']

# Remove the phrases from all string columns
df = df.applymap(
    lambda x: ''.join(x.replace(phrase, '') for phrase in phrases_to_remove)
    if isinstance(x, str) else x
)

# Save the cleaned DataFrame back to a CSV
df.to_csv('cleaned_file.csv', index=False)

```

The training file also must be formatted in a specific way that the trainer function can read. This format can be attained throuogh somthing like this, where the text between two characters is extracted and formatted.

In [format_training_data.py](Chatbot_code/format_training_data.py), the text between "{" and "}" is saved into an array. The value of the each array element is written into the new file alternating 'user input' (- -) and 'chat bot response'(   -).


Check out the provided example by running [format_training_data.py](Chatbot_code/format_training_data.py) to format [health_training_data2.csv](Chatbot_code/health_training_data2.csv) into a useable training file.




### Untraining
Everything that the chatbot learns is stored in its memory even after the program ends. If you are running the training programs and realize you trained the bot with something you do not want it to know/ talk to users about, you can run the [```untrain.py```](Chatbot_code/untrain.py) file to reset its memory.


# 5. Using The Chat Bot
1. Run ```bot.py``` to initiate chat bot.
2. Allow training data to download completely.
3. Once input prompt ">" appears you can type your question/ into the terminal and hit "Enter" to send your input to the chat bot.
4. Keep on chatting!

### Tips to use
* Provide as much context as possible in your input (i.e. no single-word responses) since the bot does not have short-term memory of what was previously said in the conversation.


<br><br><br><br>
# Additional Resources
* [Setting up a python environment](https://code.visualstudio.com/docs/python/environments)
* [Free chat bot tools](https://www.edenai.co/post/top-free-chatbot-tools-apis-and-open-source-models): These are other resources that can be used to build a chat bot.
