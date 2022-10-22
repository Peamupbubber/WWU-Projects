/*
 * I used this link to write the ceiling function for the takeOffTIme:
 * https://stackoverflow.com/questions/45252229/ceiling-rounded-to-multiple-of-60-java
 * I changed their method a little bit but I still used their concept
*/

public class Schedule {
    String flightNum;
    String from;
    String to;

    String takeOffRequestTime;
    int takeOffRequestTimeInt;
    int takeOffTime;
    
    int numPassengers;
    int positionAdded;

    public Schedule(String flightNum, String from, String to, String takeOffRequestTime, int numPassengers, int positionAdded) {
        this.flightNum = flightNum;
        this.from = from;
        this.to = to;
        
        //Creates the minutes and hours of takeOffRequestTimeInt assuming it was input correctly
        //e.g. 12:05
        String[] timeStrings = takeOffRequestTime.split(":");
        int hour = Integer.parseInt(timeStrings[0]);
        int min = Integer.parseInt(timeStrings[1]);
        if(hour < 12)
            hour += 12;
        takeOffRequestTimeInt = hour * 60 + min;

        this.takeOffTime = ceiling(numPassengers / 2)/60;

        this.numPassengers = numPassengers;
        this.positionAdded = positionAdded;
    }

    public String toString(int type, int time) {
        if(type == 1)
            return flightNum;
        else if(type == 2)
            return flightNum + " " + numPassengers;
        else if(type == 3) {

            //Unneeded because I gave up on type 3 implementation
            int hour = (int)(time/60);
            if(hour > 12)
                hour -= 12;

            int min = (int)(time%60);
            String minString = String.valueOf(min);
            if(min < 10)
                minString = "0" + min;

            String takeOffTimeString = hour + ":" + minString;

            return flightNum + " departed at " + takeOffTimeString;
        }
        else
            return "";
    }

    private int ceiling(int num) {
        int remainder = num % 60;
        if(remainder == 0)
            return num;
        return num - remainder + 60;
      }
}