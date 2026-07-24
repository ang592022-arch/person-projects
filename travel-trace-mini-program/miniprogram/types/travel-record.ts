export type TravelMediaType = 'image' | 'video';
export type TravelTicketType = 'flight' | 'train' | 'highSpeedRail';
export type TravelVisibility = 'private' | 'shareable';

export interface TravelImage {
  fileID: string;
  cloudPath?: string;
  tempPath?: string;
  localPath?: string;
  mediaType?: TravelMediaType;
  size?: number;
  duration?: number;
  uploadedAt: string;
}

export interface TravelTicket {
  id: string;
  type: TravelTicketType;
  title: string;
  departure: string;
  arrival: string;
  departAt?: string;
  arriveAt?: string;
  carrier?: string;
  ticketNo?: string;
  seat?: string;
  note?: string;
}

export interface TravelRecord {
  id: string;
  regionCode: string;
  visitDate: string;
  title?: string;
  content: string;
  tags: string[];
  companions: string[];
  visibility: TravelVisibility;
  images: TravelImage[];
  tickets: TravelTicket[];
  createdAt: string;
  updatedAt: string;
}

export interface TravelRecordInput {
  id?: string;
  regionCode: string;
  visitDate: string;
  title?: string;
  content: string;
  tags: string[];
  companions?: string[];
  visibility?: TravelVisibility;
  images: TravelImage[];
  tickets?: TravelTicket[];
}
