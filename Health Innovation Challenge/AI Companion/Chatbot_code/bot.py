'''
Author:Maggie Crawford
Date: 12th Nov , 2024
Project: Health Hub -Health Companion

Solution provides base AI chat bot structure and training format to create a Mental Health resource companion
'''

from chatterbot import ChatBot
import nltk
nltk.download('punkt_tab')
from chatterbot.trainers import ListTrainer
import train

chatbot_name = "Your Companion"
chatbot = ChatBot(chatbot_name)
# trainer = ListTrainer(chatbot)


##Run Training Protcols from train.py
# train.introduce_yourself()
train.trainbot()

##Chat-Response loop
while True:
    query = input(">  ")
    if query in train.exit_conditions:
        break
    else:
        print(f">>>>: {chatbot.get_response(query)}")