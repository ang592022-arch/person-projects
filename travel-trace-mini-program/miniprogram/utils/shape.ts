import type { RegionShape } from '../types/region';

export function shapeToClipPath(shape: RegionShape): string {
  if (shape.type === 'rect') {
    return 'inset(0 round 8px)';
  }

  const points = shape.points || [];
  return `polygon(${points.map(([x, y]) => `${x}px ${y}px`).join(', ')})`;
}

export function shapeToStyle(shape: RegionShape, mapWidth: number, mapHeight: number): string {
  if (shape.type === 'rect') {
    return [
      `left:${((shape.x || 0) / mapWidth) * 100}%`,
      `top:${((shape.y || 0) / mapHeight) * 100}%`,
      `width:${((shape.width || 0) / mapWidth) * 100}%`,
      `height:${((shape.height || 0) / mapHeight) * 100}%`,
      'clip-path:inset(0 round 8rpx)',
      '-webkit-clip-path:inset(0 round 8rpx)',
    ].join(';');
  }

  const points = shape.points || [];
  if (points.length === 0) return '';

  const xs = points.map(([x]) => x);
  const ys = points.map(([, y]) => y);
  const minX = Math.min(...xs);
  const maxX = Math.max(...xs);
  const minY = Math.min(...ys);
  const maxY = Math.max(...ys);
  const width = maxX - minX || 1;
  const height = maxY - minY || 1;
  const localPoints = points.map(([x, y]) => `${((x - minX) / width) * 100}% ${((y - minY) / height) * 100}%`);

  return [
    `left:${(minX / mapWidth) * 100}%`,
    `top:${(minY / mapHeight) * 100}%`,
    `width:${((maxX - minX) / mapWidth) * 100}%`,
    `height:${((maxY - minY) / mapHeight) * 100}%`,
    `clip-path:polygon(${localPoints.join(',')})`,
    `-webkit-clip-path:polygon(${localPoints.join(',')})`,
  ].join(';');
}

export function labelToStyle(center: [number, number], mapWidth: number, mapHeight: number): string {
  return [
    `left:${(center[0] / mapWidth) * 100}%`,
    `top:${(center[1] / mapHeight) * 100}%`,
  ].join(';');
}
