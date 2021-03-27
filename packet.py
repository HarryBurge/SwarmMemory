class Packet:

    def __init__(self, type, sendid, recid, additional=None, data=None):

        self.sendid = sendid
        self.recid = recid
        self.type = type
        self.additional = additional
        self.data = data