package module3_milestone;

import java.util.concurrent.atomic.AtomicLong;

public class Contact {
	
	private String firstName;
	private String lastName;
	private String phoneNum;
	private String address;
	private final String contactId;
	private static AtomicLong idGenerator = new AtomicLong();
	
	
	// Constructor with parameters
	
	public Contact(String firstName, String lastName, String phoneNum, String address) {
		
		//Assigns unique Id for each instance created - Does not duplicate Id numbers
		this.contactId = String.valueOf(idGenerator.getAndIncrement());
		
		this.setFirstName(firstName);

		this.setLastName(lastName);

		this.setPhoneNum(phoneNum);

		this.setAddress(address);

	}
	
public Contact(String contactId, String firstName, String lastName, String phoneNum, String address) {
		
		//Assigns unique Id for each instance created - Does not duplicate Id numbers
		this.contactId = String.valueOf(idGenerator.getAndIncrement());
		
		this.setFirstName(firstName);

		this.setLastName(lastName);

		this.setPhoneNum(phoneNum);

		this.setAddress(address);

	}
	
	
	
	//Getters
	
	public String getFirstName() {
		return firstName;
	}

	public String getLastName() {
		return lastName;
	}

	public String getPhoneNum() {
		return phoneNum;
	}

	public String getAddress() {
		return address;
	}

	public String getContactId() {
		return contactId;
	}

	// Setters
	
	public void setFirstName(String firstName) {
		//Checks to ensure name is not null or is not longer than 10 characters before assigning name
		if (firstName == null || firstName.length() > 10) {
			throw new IllegalArgumentException("Invalid first name");
		}
		this.firstName = firstName;
	}
	public void setLastName(String lastName) {
		//Checks to ensure name is not null or is not longer than 10 characters before assigning name
		if (lastName == null || lastName.length() > 10) {
			throw new IllegalArgumentException("Invalid last name");
		}
		this.lastName = lastName;
	}

	public static boolean isNumeric(String str) {
	    try {
	        Double.parseDouble(str);
	        return true;
	    } catch (NumberFormatException nfe) {
	        return false;
	    }
	}

	public void setPhoneNum(String phoneNum) {
		//Checks to ensure phone number is not null or that it has exactly 10 digits before assigning phone number
		
		if (phoneNum == null || phoneNum.length() != 10 || isNumeric(phoneNum) == false) {
			throw new IllegalArgumentException("Invalid phone number");
		}
		this.phoneNum =phoneNum;
	}
	
	public void setAddress(String address) {
		//Checks to ensure address is not null or that it has less than 30 characters before assigning address
		if (address == null || address.length() > 30) {
			throw new IllegalArgumentException("Invalid address"); 
		}
		this.address = address;
	}
	

}
