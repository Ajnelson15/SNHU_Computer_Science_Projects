package test;

import static org.junit.Assert.assertTrue;
import static org.junit.jupiter.api.Assertions.*;

import java.util.Objects;

import org.junit.jupiter.api.MethodOrderer.OrderAnnotation;
import org.junit.jupiter.api.Order;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.TestMethodOrder;

import module3_milestone.ContactService;


//Test Created for testing ContactService.java

@TestMethodOrder(OrderAnnotation.class)
class TestContactService {

	@Test
	@Order(1)
	void testAddContact() {
		var service = new ContactService(); service.addContact("Michael", "Jordon" ,"6083332222" , "Chicago Illinois");
		assertTrue(service.contactList.get(0).getFirstName().equals("Michael"));
		assertTrue(service.contactList.get(0).getLastName().equals("Jordon"));
		assertTrue(service.contactList.get(0).getPhoneNum().equals("6083332222"));
		assertTrue(service.contactList.get(0).getAddress().equals("Chicago Illinois"));
		service.displayContactList();
	}
	 
	@Test
	@Order(2)
	void testDeleteContact() {
		var service = new ContactService();
		service.addContact("Larry", "Bird" , "6083335555" , "Boston Massachusetts");
		assertTrue(service.contactList.get(0).getFirstName().equals("Larry"));
		assertTrue(service.contactList.get(0).getLastName().equals("Bird"));
		assertTrue(service.contactList.get(0).getPhoneNum().equals("6083335555"));
		assertTrue(service.contactList.get(0).getAddress().equals("Boston Massachusetts"));
		service.displayContactList();
		service.deleteContact("1");
		System.out.println("...Deleting Larry Bird Contact...\n\n");
		service.displayContactList();
		assertNull(service.getContact("1"));	
	}
	
	@Test
	@Order(3)
	void testUpdateFirstName() {
		var service = new ContactService();
		service.addContact("Lew", "Alcinder" , "9991117777" , "Houston Texas");
		service.updateFirstName("Kareem", "2");
		assertTrue(service.contactList.get(0).getFirstName().equals("Kareem"));
		service.displayContactList();
	}
	
	@Test
	@Order(4)
	void testUpdateLastName() {
		var service = new ContactService();
		service.addContact("Ron", "Artest" , "2459998754" , "Los Angelas California");
		service.updateLastName("WorldPeace", "3");
		assertTrue(service.contactList.get(0).getLastName().equals("WorldPeace"));
		service.displayContactList();
	}
	
	@Test
	@Order(5)
	void testUpdateNumber() {
		var service = new ContactService();
		service.addContact("Kobe", "Bryant" , "9998882424" , "Los Angelas California");
		service.updateNumber("1112223333", "4");
		assertTrue(service.contactList.get(0).getPhoneNum().equals("1112223333"));
		service.displayContactList();
	}
	
	@Test
	@Order(6)
	void testUpdateAddress() {
		var service = new ContactService();
		service.addContact("Shaquille", "O' Neil" , "5648789858" , "Miami Florida");
		service.updateAddress("Cleveland Ohio", "5");
		service.displayContactList();
	}
	
	@Test
	@Order(7)
	void testGetContact() { 
		var service = new ContactService();
		service.addContact("Penny", "Hardaway" , "8771119808" , "Orlando Florida");
		assertNotNull(service.getContact("6"));
		service.displayContactList();
	}
	@Test
	@Order(8)
	void testForUniqueId() {
		
		var service = new ContactService();
		service.addContact("Aaaaa", "Hhhhh" , "8771111424" , "Thailand");
		service.addContact("Bbbbb", "Iiiii" , "8771118764" , "Japan");
		service.addContact("Ccccc", "Jjjjj" , "8771117985" , "Canada");
		service.addContact("Ddddd", "Kkkkk" , "8771111122" , "Mexico");
		service.addContact("Eeeee", "Lllll" , "8771119866" , "Australia");
		service.addContact("Fffff", "Mmmmm" , "8771115766" , "New Zealand");
		service.addContact("Ggggg", "Nnnnn" , "8771113452" , "Brazil");
		service.displayContactList();
		
		//Iterates through the contact list looking to verify each id is unique from the rest in the list
		for (int i = 0; i < service.contactList.size(); ++i) {
			for (int j = i + 1; j < service.contactList.size(); ++j) {
				assertFalse(Objects.equals(service.contactList.get(i).getContactId(), service.contactList.get(j).getContactId()));
				//System.out.println("i = " + service.contactList.get(i).getContactId() + "\n j = " + service.contactList.get(j).getContactId());
			}
		}		
	}
}
