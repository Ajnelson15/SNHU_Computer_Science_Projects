import pprint #formats printing
from pymongo import MongoClient
from bson.objectid import ObjectId



class AnimalShelter(object):
    """ CRUD operations for Animal collection in MongoDB """


    def __init__(self, username, password):
        # Initializing the MongoClient. This helps to 
        # access the MongoDB databases and collections.
        # This is hard-wired to use the aac database, the 
        # animals collection, and the aac user.
        # Definitions of the connection string variables are
        # unique to the individual Apporto environment.
        #
        # You must edit the connection variables below to reflect
        # your own instance of MongoDB!
        #
        # Connection Variables
        #
        HOST = 'nv-desktop-services.apporto.com'
        PORT = 31602
        DB = 'AAC'
        COL = 'animals'
        #
        # Initialize Connection
        #
        self.client = MongoClient('mongodb://%s:%s@%s:%d' % (username,password,HOST,PORT))
        self.database = self.client['%s' % (DB)]
        self.collection = self.database['%s' % (COL)]



# Create Method - Used to insert new data.
    def create(self, data):
        
        # Checks data parameter to ensure it's not empty
        if data:
            # Tries to insert data into datbase and store it in result
            try:
                result = self.database.animals.insert_one(data)
                print("Data has successfully been inserted \n")
                
                #Assigns True for successfull insertion and False for unsuccessful insertion
                if result.inserted_id:
                    return True
                else:
                    return False
            #Catches exceptions and prints them
            except Exception as e:
                print("An error has occured:\n" + "--" + str(e) + "\n")

        else:
            raise Exception("The data parameter is empty")
    
           

#Read method - Used to search for data.
    def read(self, data = None):
        # Checks data parameter to ensure it's not empty
        try:
            result = list(self.database.animals.find(data))
            return result
        except Exception as e:
            print("An error has occured:\n" + "--" + str(e) + "\n")
        

# Read method - Used to search for data.
    def readOne(self, data):
        # Checks data parameter to ensure it's not empty
        if data:
            # Tries to read data into datbase and store it in result
            try:
                result = self.database.animals.find(data)
                return result
            # Catches exceptions and prints them out
            except Exception as e:
                print("An error has occured:\n" + "--" + str(e) + "\n")
        else:
            raise Exception("The data parameter is empty")
           
            
# Update method - Used to update one queried data.
    def updateOne(self, data, update):
        # Checks data parameter to ensure it's not empty
        if data:
            # Tries to update data in database and stores it in result
            try:
                result = self.database.animals.update_one(data, update)
                
                # Stores matched count and modified count in a string
                match = str(result.matched_count)
                mod = str(result.modified_count)
                
                # Prints number of matches and the number of modified documents
                print("Documents found that match: " + match )
                print("\nDocuments modified: " + mod )

            # Catches exceptions and prints them out
            except Exception as e:
                print("An error has occured:\n" + "--" + str(e) + "\n")
        else:
            raise Exception("The data parameter is empty")
            
            
# UpdateMany - Used to update all queried data.
    def updateMany(self, data, update):
        # Checks data parameter to ensure it's not empty
        if data:
            # Tries to update data in database and stores it in result
            try:
                result = self.database.animals.update_many(data, update)
                
                # Stores matched count and modified count in a string
                match = str(result.matched_count)
                mod = str(result.modified_count)
                
                # Prints number of matches and the number of modified documents
                print("Documents found that match: " + match )
                print("\nDocuments modified: " + mod )

            # Catches exceptions and prints them out
            except Exception as e:
                print("An error has occured:\n" + "--" + str(e) + "\n")
        else:
            raise Exception("The data parameter is empty")

# DeleteOne method - Used to delete one queried data.
    def deleteOne(self, data):
        # Checks data parameter to ensure it's not empty
        if data:
            # Tries to delete data in database and stores it in result
            try:
                result = self.database.animals.delete_one(data)
                
                # Stores numer of deleted documents and prints it out
                count = str(result.deleted_count)
                print(count + " document deleted")
                
            
            # Catches exceptions and prints them out
            except Exception as e:
                print("An error has occured:\n" + "--" + str(e) + "\n")
        else:
            raise Exception("The data parameter is empty")
            
# DeleteMany method - Used to delete  queried data.
    def deleteMany(self, data):
        # Checks data parameter to ensure it's not empty
        if data:
            # Tries to delete data in database and stores it in result
            try:
                result = self.database.animals.delete_many(data)
                
                # Stores numer of deleted documents and prints it out
                count = str(result.deleted_count)
                print(count + " document(s) deleted")
                
            # Catches exceptions and prints them out
            except Exception as e:
                print("An error has occured:\n" + "--" + str(e) + "\n")
        else:
            raise Exception("The data parameter is empty")