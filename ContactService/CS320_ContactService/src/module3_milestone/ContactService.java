package module3_milestone;

import java.util.ArrayList;
import java.util.List; 

public class ContactService {
	
	//Array List - Contact List - to store contacts in
	public List<Contact> contactList = new ArrayList<Contact>();
	
	//Output display for contact information
	public void displayContactList() {
		for (int i = 0; i < contactList.size(); ++i) {
			System.out.println("Customer ID: " + contactList.get(i).getContactId());		
			System.out.println("             " + contactList.get(i).getFirstName() + " " + contactList.get(i).getLastName());
			System.out.println("             " + contactList.get(i).getPhoneNum());
			System.out.println("             " + contactList.get(i).getAddress());
			System.out.println("\n");
		
		}
	}
	//Searches for contact by using ID number
	public Contact getContact(String id) {
		for (Contact contact : contactList) {
			if(contact.getContactId().equals(id)) {
				return contact;
			}
		} 
		return null;
	}
	//Add contact to the contact list
	public void addContact(String firstName, String lastName, String number, String address) {
		Contact contact = new Contact(firstName, lastName, number, address);
		contactList.add(contact);
		
	}
	//Deletes contact from list by referencing the ID number
	public void deleteContact(String contactId) {
		this.contactList.remove(this.getContact(contactId));
			
	}
	//Updates first name by referencing the ID number
	public void updateFirstName(String updatedFirstName, String contactId) {
		Contact contact = getContact(contactId);
		contact.setFirstName(updatedFirstName);
	}
	//Updates last name by referencing the ID number
	public void updateLastName(String updatedLastName, String contactId) {
		Contact contact = getContact(contactId);
		contact.setLastName(updatedLastName);
	}
	//Updates the phone number by referencing the ID number
	public void updateNumber(String updatedNumer, String contactId) {
		Contact contact = getContact(contactId);
		contact.setPhoneNum(updatedNumer);
	}
	//Updates the address by referencing the ID number
	public void updateAddress(String updatedAddress, String contactId) {
		Contact contact = getContact(contactId);
		contact.setAddress(updatedAddress);		
		
	}
}