const TOKEN = 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx';
const ATTR_HEADER_ROW = 1;
const FISRT_RECORD_ROW = 3;

function doGet(e) {
  return createResponse('Hello');
}

function doPost(e) {
  if (e == null || e.postData == null || e.postData.contents == null) {
    return createResponse('Invalid Contents');
  }
  let requestJSON = e.postData.contents;
  let requestObj = JSON.parse(requestJSON);
  if (requestObj['token'] != TOKEN) {
    return createResponse('Mismatched Token');
  }

  let ss = SpreadsheetApp.getActive()
  let sheet = ss.getActiveSheet();
  let headers = sheet.getRange(ATTR_HEADER_ROW, 1, 1, sheet.getLastColumn()).getValues()[0];
  let numItems = requestObj['records'].length;
  if (numItems == 0) {
    return createResponse('No records');
  }

  let lastDateCell = sheet.getRange(sheet.getLastRow(),1);
  let recordTime = (new Date(lastDateCell.getValue())).getTime();
  let timeGap = ((new Date()).getTime() - recordTime) / numItems;

  requestObj['records'].map(function(record) {
    let values = [];
    recordTime += timeGap;
    headers.map(function(header) {
      let value = '';
      switch(header) {
        case 'date':
          value = new Date(recordTime);
          break;
        case 'mimeType':
          value = e.postData.type;
          break;
        default:
          value = record[header];
          break;
      }
      values.push(value);
    });
    sheet.appendRow(values);
  });
  sheet.getRange(sheet.getLastRow() - numItems + 1, 1, numItems, 1).setNumberFormat('yy/M/d H:mm');
  sheet.deleteRows(FISRT_RECORD_ROW, numItems);

  return createResponse("Success");
}

function createResponse(msg) {
  let output = ContentService.createTextOutput(msg);
  output.setMimeType(ContentService.MimeType.TEXT);
  return output;
}

function doPostTest() {
  let records = [];
  for (let i = 0; i < 5; i++) {
    records.push({
      illum: 100 + Math.random() * 200,
      temp: 20 + Math.random() * 10
    });
  } 
  let requestObj = {
    token: TOKEN,
    records: records
  };
  let postData = new Object();
  postData.contents = JSON.stringify(requestObj);
  postData.type = 'application/json'; // ContentService.MimeType.JSON;
  let e = new Object();
  e.postData = postData;
  doPost(e);
}
