package test;

import static org.junit.Assert.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

import module3_milestone.Contact;

//Tests created to check Contact.java
class ContactTest {

	@Test
	void testContactClass() {
		Contact contact = new Contact( "Andrew" , "Nelson", "6083339999" , "111 Apple Valley" );
		assertTrue(contact.getFirstName().equals("Andrew"));
		assertTrue(contact.getLastName().equals("Nelson"));
		assertTrue(contact.getPhoneNum().equals("6083339999"));
		assertTrue(contact.getAddress().equals("111 Apple Valley"));
		assertFalse(contact.getContactId().length() > 10);
		assertFalse(contact.getContactId().equals(null));		
	}
	
	@Test
	void testContactIDCantBeUpdated() {
		Contact contact = new Contact("12345678", "Andrew" , "Nelson", "6083339999" , "111 Apple Valley" );
		assertFalse(contact.getContactId().equals("12345678") );
		assertNotNull(contact.getContactId());		
	}
	
	@Test
	void testContactFirstNameTooLong() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact("MyNameisTooLong" , "Nelson", "6083339999" , "111 Apple Valley");
		});	}
	
	@Test
	void testContactFirstName_CanBe_EqualTo10_Characters() {
		Contact contact = new Contact("TenCharact", "Nelson","6083339999", "111 Apple Valley");
		assertEquals(contact.getFirstName().length(),10);
	}
	
	@Test
	void testContactFirstName_CanBe_LessThan10_Characters() {
		Contact contact = new Contact("Nine_Char", "Nelson","6083339999", "111 Apple Valley");
		assertTrue(contact.getFirstName().length() < 10 );
	}
	
	@Test
	void testContactFirstNameNull() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact(null , "Nelson", "6083339999" , "111 Apple Valley");
		});	}
	
	@Test
	void testContactLastNameTooLong() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact("Andrew" , "MyNameIsTooLong", "6083339999" , "111 Apple Valley");
		});	}
	
	@Test
	void testContactLasttName_CanBe_EqualTo10_Characters() {
		Contact contact = new Contact("Andrew", "TenCharact","6083339999", "111 Apple Valley");
		assertEquals(contact.getLastName().length(),10);
	}
	
	@Test
	void testContactLastName_CanBe_LessThan10_Characters() {
		Contact contact = new Contact("Andrew", "Nine_Char","6083339999", "111 Apple Valley");
		assertTrue(contact.getFirstName().length() < 10);
	}
		
	@Test
	void testContactLastNameNull() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact( "Andrew" , null , "6083339999" , "111 Apple Valley");
		});	}
	
	@Test
	void testContactPhoneNumberIsDigits() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact( "Andrew" , "Nelson" , "60833a9799" , "111 Apple Valley");
		});	}
	
	@Test
	void testContactTooShortPhoneNumber() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact("Andrew" , "Nelson", "TOOSHORT" , "111 Apple Valley");
		});	}
	
	@Test
	void testContactTooLongPhoneNumber() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact("Andrew" , "Nelson", "TOO_ _ _LONG" , "111 Apple Valley");
		});	}
	
	@Test
	void testContactPhoneNumberNull() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact( "Andrew" , "Nelson" , null , "111 Apple Valley");
		});	}
	@Test
	void testContactAddressTooLong() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact("Andrew" , "Nelson", "6083339999" , "MyAddressIsMuchLongerThanItIsAllowedToBe");
		});	}
	
	@Test
	void testContactAddressNull() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact( "Andrew" , "Nelson" , "6083339999" , null);
		});	}
	}